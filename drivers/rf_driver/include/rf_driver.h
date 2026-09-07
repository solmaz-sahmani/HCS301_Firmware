#ifndef RF_DRIVER_H
#define RF_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#include "err_core.h"

typedef struct
{
    bool level;
    uint16_t duration_us;
} rf_pulse_t;

typedef status_t (*rf_hal_transmit_fn)(
    void *context,
    const rf_pulse_t *pulses,
    uint32_t count);

typedef struct
{
    rf_hal_transmit_fn transmit;
    void *context;
} rf_hal_t;

typedef struct rf_driver rf_driver_t;

rf_driver_t *rf_driver_get_instance(void);

/**
 * @brief Initialize RF driver.
 */
status_t rf_driver_init(
    rf_driver_t *driver,
    const rf_hal_t *hal);

/**
 * @brief Transmit RF pulses.
 */
status_t rf_driver_transmit(
    rf_driver_t *driver,
    const rf_pulse_t *pulses,
    uint32_t count);

#endif