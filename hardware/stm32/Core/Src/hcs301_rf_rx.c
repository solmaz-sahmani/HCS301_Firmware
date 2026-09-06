#include "hcs301_rf_rx.h"

#include <string.h>

static bool in_range(uint32_t value, uint32_t target, uint32_t tolerance)
{
    return value >= (target - tolerance) &&
           value <= (target + tolerance);
}

static bool is_te(uint32_t value)
{
    return in_range(
        value,
        HCS301_RF_RX_TE_US,
        HCS301_RF_RX_TOLERANCE_US);
}

static bool is_2te(uint32_t value)
{
    return in_range(
        value,
        2U * HCS301_RF_RX_TE_US,
        2U * HCS301_RF_RX_TOLERANCE_US);
}

static bool is_header(uint32_t value)
{
    return value >= HCS301_RF_RX_HEADER_MIN_US &&
           value <= HCS301_RF_RX_HEADER_MAX_US;
}

static bool is_guard(uint32_t value)
{
    return value >= HCS301_RF_RX_GUARD_MIN_US;
}

HAL_StatusTypeDef hcs301_rf_rx_init(hcs301_rf_rx_t *rx)
{
    if (rx == NULL)
    {
        return HAL_ERROR;
    }

    memset(rx, 0, sizeof(*rx));
    return HAL_OK;
}

HAL_StatusTypeDef hcs301_rf_rx_start(
    hcs301_rf_rx_t *rx,
    TIM_HandleTypeDef *htim)
{
    if (rx == NULL || htim == NULL)
    {
        return HAL_ERROR;
    }

    return HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_1);
}

void hcs301_rf_rx_capture_edge(
    hcs301_rf_rx_t *rx,
    TIM_HandleTypeDef *htim)
{
    uint32_t current;
    uint32_t duration;
    GPIO_PinState level;

    if (rx == NULL || htim == NULL)
    {
        return;
    }

    current = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    level = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);

    if (!rx->have_previous_capture)
    {
        rx->previous_capture = current;
        rx->have_previous_capture = true;
        return;
    }

    if (current >= rx->previous_capture)
    {
        duration = current - rx->previous_capture;
    }
    else
    {
        duration =
            (0xFFFFFFFFU - rx->previous_capture) +
            current +
            1U;
    }

    rx->previous_capture = current;

    /*
     * A long low interval is the guard time. Reset and wait
     * for the next preamble/header.
     */
    if (is_guard(duration))
    {
        rx->bit_count = 0U;
        rx->have_high_duration = false;
        rx->receiving = false;
        return;
    }

    /*
     * The falling edge after the 10 TE header produces a long
     * high interval. This is our frame synchronization point.
     */
    if (!rx->receiving)
    {
        if (level == GPIO_PIN_RESET && is_header(duration))
        {
            rx->bit_count = 0U;
            rx->have_high_duration = false;
            rx->receiving = true;
        }

        return;
    }

    /*
     * Both-edge capture:
     *   falling edge -> the preceding interval was HIGH
     *   rising edge  -> the preceding interval was LOW
     */
    if (level == GPIO_PIN_RESET)
    {
        if (is_te(duration) || is_2te(duration))
        {
            rx->high_duration = duration;
            rx->have_high_duration = true;
        }
        else
        {
            rx->bit_count = 0U;
            rx->have_high_duration = false;
            rx->receiving = false;
        }

        return;
    }

    if (!rx->have_high_duration)
    {
        return;
    }

    if (is_te(rx->high_duration) && is_2te(duration))
    {
        rx->bits[rx->bit_count++] = 0U;
    }
    else if (is_2te(rx->high_duration) && is_te(duration))
    {
        rx->bits[rx->bit_count++] = 1U;
    }
    else
    {
        rx->bit_count = 0U;
        rx->have_high_duration = false;
        rx->receiving = false;
        return;
    }

    rx->have_high_duration = false;

    if (rx->bit_count == HCS301_RF_RX_BITS)
    {
        rx->frame_ready = true;
        rx->receiving = false;
    }
}

bool hcs301_rf_rx_frame_ready(const hcs301_rf_rx_t *rx)
{
    return rx != NULL && rx->frame_ready;
}

bool hcs301_rf_rx_read(
    hcs301_rf_rx_t *rx,
    uint8_t bits[HCS301_RF_RX_BITS])
{
    if (rx == NULL || bits == NULL || !rx->frame_ready)
    {
        return false;
    }

    memcpy(bits, rx->bits, HCS301_RF_RX_BITS);

    rx->frame_ready = false;
    rx->bit_count = 0U;
    rx->have_high_duration = false;
    rx->receiving = false;

    return true;
}
