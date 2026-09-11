#ifndef RF_STM32_H
#define RF_STM32_H

#include "rf_driver.h"

/**
 * @brief Initialize the STM32 RF receiver backend.
 *
 * The receiver uses TIM3 CH1 on PA6
 * to capture the pulse widths generated
 * by the FS1000A receiver module.
 *
 * @param hal RF HAL interface.
 *
 * @return STATUS_OK on success.
 */
status_t rf_stm32_init(rf_hal_t *hal);

/**
 * @brief Process a captured RF edge.
 *
 * This function is called from the timer
 * input-capture interrupt.
 *
 * @param capture_value Current timer capture value.
 */
void rf_stm32_capture_callback(uint16_t capture_value);

/**
 * @brief Get the currently captured RF frame.
 *
 * @param pulses Destination pulse buffer.
 * @param max_count Maximum number of pulses.
 * @param count Number of captured pulses.
 *
 * @return STATUS_OK on success.
 */
status_t rf_stm32_receive_read(
    rf_pulse_t *pulses,
    uint32_t max_count,
    uint32_t *count);

#endif