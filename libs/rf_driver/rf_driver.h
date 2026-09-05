#ifndef RF_DRIVER_H
#define RF_DRIVER_H

#include <stdint.h>

#include "err_core.h"

#define RF_MAX_PULSES 128U

typedef struct
{
    uint32_t duration_us;
    uint8_t level;

} rf_pulse_t;

typedef struct rf_driver rf_driver_t;

/**
 * @brief Get the RF driver instance.
 *
 * @return RF driver instance.
 */
rf_driver_t *rf_driver_get_instance(void);

/**
 * @brief Initialize RF driver.
 *
 * @param driver RF driver instance.
 *
 * @return Operation status.
 */
status_t rf_driver_init(
    rf_driver_t *driver);

/**
 * @brief Transmit RF pulses.
 *
 * @param driver RF driver instance.
 * @param pulses Pulse array.
 * @param count Number of pulses.
 *
 * @return Operation status.
 */
status_t rf_driver_transmit(
    rf_driver_t *driver,
    const rf_pulse_t *pulses,
    uint32_t count);

/**
 * @brief Receive RF pulses.
 *
 * @param driver RF driver instance.
 * @param pulses Output pulse array.
 * @param count Maximum pulse count.
 * @param received Output received pulse count.
 *
 * @return Operation status.
 */
status_t rf_driver_receive(
    rf_driver_t *driver,
    rf_pulse_t *pulses,
    uint32_t count,
    uint32_t *received);

#endif