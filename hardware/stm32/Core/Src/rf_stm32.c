#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "stm32f0xx_hal.h"

#include "rf_stm32.h"

/*
 * RF timing configuration.
 *
 * Timer resolution:
 *
 *     1 tick = 1 us
 *
 * HCS301:
 *
 *     TE = 400 us
 *
 * Therefore:
 *
 *     1 TE = 400 timer ticks
 */

#define RF_CAPTURE_BUFFER_SIZE    128U

#define RF_TE_US                   400U

/*
 * A pulse longer than this is considered
 * a frame gap / synchronization gap.
 */
#define RF_FRAME_GAP_US            8000U

/*
 * Maximum accepted pulse duration.
 */
#define RF_MAX_PULSE_US            16000U

static rf_pulse_t capture_buffer[RF_CAPTURE_BUFFER_SIZE];

static volatile uint32_t capture_count = 0U;

static volatile uint16_t previous_capture = 0U;

static volatile bool capture_active = false;

static volatile bool frame_ready = false;


/**
 * @brief Initialize RF receiver state.
 */
status_t rf_stm32_init(rf_hal_t *hal)
{
    if (hal == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    capture_count = 0U;
    previous_capture = 0U;
    capture_active = false;
    frame_ready = false;

    /*
     * The actual TIM3 configuration is performed
     * in the STM32 HAL initialization code.
     */

    hal->transmit = NULL;
    hal->receive_start = NULL;
    hal->receive_stop = NULL;

    /*
     * The RF driver will use this function
     * to read captured pulses.
     */
    hal->receive_read =
        (rf_hal_receive_read_fn)rf_stm32_receive_read;

    hal->context = NULL;

    return STATUS_OK;
}


/**
 * @brief Handle a timer input-capture event.
 *
 * This function must be called from:
 *
 *     HAL_TIM_IC_CaptureCallback()
 *
 * when TIM3 CH1 captures a rising/falling edge.
 */
void rf_stm32_capture_callback(uint16_t capture_value)
{
    uint16_t duration;

    /*
     * Calculate timer difference.
     *
     * uint16_t arithmetic naturally handles
     * timer overflow.
     */
    duration =
        (uint16_t)(capture_value - previous_capture);

    previous_capture = capture_value;

    /*
     * Ignore invalid pulses.
     */
    if (duration == 0U)
    {
        return;
    }

    /*
     * A very long pulse means that the RF frame
     * has ended.
     */
    if (duration > RF_FRAME_GAP_US)
    {
        if (capture_count > 0U)
        {
            frame_ready = true;
        }

        capture_active = false;

        return;
    }

    /*
     * Ignore pulses that are clearly invalid.
     */
    if (duration > RF_MAX_PULSE_US)
    {
        return;
    }

    /*
     * Start a new frame.
     */
    if (!capture_active)
    {
        capture_count = 0U;
        capture_active = true;
    }

    /*
     * Store the pulse.
     *
     * The level is not explicitly stored here because
     * this simple receiver assumes alternating
     * rising/falling edges.
     *
     * The decoder reconstructs the level sequence.
     */
    if (capture_count < RF_CAPTURE_BUFFER_SIZE)
    {
        capture_buffer[capture_count].duration_us =
            duration;

        capture_buffer[capture_count].level =
            ((capture_count & 1U) == 0U);

        capture_count++;
    }
}


/**
 * @brief Read a completed RF frame.
 */
status_t rf_stm32_receive_read(
    rf_pulse_t *pulses,
    uint32_t max_count,
    uint32_t *count)
{
    if (pulses == NULL || count == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    *count = 0U;

    if (!frame_ready)
    {
        return STATUS_ERROR;
    }

    uint32_t copy_count = capture_count;

    if (copy_count > max_count)
    {
        copy_count = max_count;
    }

    for (uint32_t i = 0U; i < copy_count; i++)
    {
        pulses[i] = capture_buffer[i];
    }

    *count = copy_count;

    /*
     * Reset receiver state for the next frame.
     */
    capture_count = 0U;
    frame_ready = false;
    capture_active = false;

    return STATUS_OK;
}