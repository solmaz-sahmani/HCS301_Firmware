#include <stddef.h>
#include <stdbool.h>

#include "rf_stm32.h"

typedef struct
{
    TIM_HandleTypeDef *timer;
    uint32_t channel;

    GPIO_TypeDef *data_port;
    uint16_t data_pin;

    rf_pulse_t pulses[RF_STM32_MAX_PULSES];

    volatile uint32_t pulse_count;
    volatile uint32_t last_capture;

    volatile bool capture_started;
    volatile bool receiving;

    volatile bool last_level;

} rf_stm32_context_t;

static rf_stm32_context_t rf_stm32_context;

static status_t rf_stm32_receive_start(
    void *context)
{
    if (context == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    rf_stm32_context_t *rf =
        (rf_stm32_context_t *)context;

    rf->pulse_count = 0U;
    rf->last_capture = 0U;
    rf->capture_started = false;
    rf->receiving = true;
    rf->last_level = false;

    if (HAL_TIM_IC_Start_IT(
            rf->timer,
            rf->channel) != HAL_OK)
    {
        rf->receiving = false;
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

static status_t rf_stm32_receive_stop(
    void *context)
{
    if (context == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    rf_stm32_context_t *rf =
        (rf_stm32_context_t *)context;

    if (HAL_TIM_IC_Stop_IT(
            rf->timer,
            rf->channel) != HAL_OK)
    {
        return STATUS_ERROR;
    }

    rf->receiving = false;

    return STATUS_OK;
}

static status_t rf_stm32_receive_read(
    void *context,
    rf_pulse_t *pulses,
    uint32_t max_count,
    uint32_t *count)
{
    if (context == NULL ||
        pulses == NULL ||
        count == NULL ||
        max_count == 0U)
    {
        return STATUS_INVALID_ARG;
    }

    rf_stm32_context_t *rf =
        (rf_stm32_context_t *)context;

    uint32_t copy_count = rf->pulse_count;

    if (copy_count > max_count)
    {
        copy_count = max_count;
    }

    for (uint32_t i = 0U; i < copy_count; i++)
    {
        pulses[i] = rf->pulses[i];
    }

    *count = copy_count;

    return STATUS_OK;
}

void HAL_TIM_IC_CaptureCallback(
    TIM_HandleTypeDef *htim)
{
    rf_stm32_context_t *rf =
        &rf_stm32_context;

    if (!rf->receiving)
    {
        return;
    }

    if (htim != rf->timer)
    {
        return;
    }

    if (htim->Channel != HAL_TIM_ACTIVE_CHANNEL_1)
    {
        return;
    }

    uint32_t capture =
        HAL_TIM_ReadCapturedValue(
            htim,
            rf->channel);

    bool current_level =
        (HAL_GPIO_ReadPin(
            rf->data_port,
            rf->data_pin) == GPIO_PIN_SET);

    /*
     * First edge only establishes
     * the initial timestamp.
     */
    if (!rf->capture_started)
    {
        rf->last_capture = capture;

        rf->last_level =
            !current_level;

        rf->capture_started = true;

        return;
    }

    uint32_t duration =
        capture - rf->last_capture;

    rf->last_capture = capture;

    if (duration == 0U)
    {
        return;
    }

    if (duration > UINT16_MAX)
    {
        rf->receiving = false;
        return;
    }

    if (rf->pulse_count >=
        RF_STM32_MAX_PULSES)
    {
        rf->receiving = false;
        return;
    }

    rf->pulses[
        rf->pulse_count
    ].level = rf->last_level;

    rf->pulses[
        rf->pulse_count
    ].duration_us = (uint16_t)duration;

    rf->pulse_count++;

    rf->last_level =
        current_level;
}

status_t rf_stm32_init(
    rf_hal_t *hal,
    const rf_stm32_config_t *config)
{
    if (hal == NULL ||
        config == NULL ||
        config->timer == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    rf_stm32_context.timer =
        config->timer;

    rf_stm32_context.channel =
        config->channel;

    rf_stm32_context.data_port =
        config->data_port;

    rf_stm32_context.data_pin =
        config->data_pin;

    rf_stm32_context.pulse_count = 0U;
    rf_stm32_context.capture_started = false;
    rf_stm32_context.receiving = false;

    hal->transmit = NULL;

    hal->receive_start =
        rf_stm32_receive_start;

    hal->receive_stop =
        rf_stm32_receive_stop;

    hal->receive_read =
        rf_stm32_receive_read;

    hal->context =
        &rf_stm32_context;

    return STATUS_OK;
}