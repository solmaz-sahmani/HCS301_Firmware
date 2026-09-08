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

typedef status_t (*rf_hal_receive_start_fn)(
    void *context);

typedef status_t (*rf_hal_receive_stop_fn)(
    void *context);

typedef status_t (*rf_hal_receive_read_fn)(
    void *context,
    rf_pulse_t *pulses,
    uint32_t max_count,
    uint32_t *count);

typedef struct
{
    rf_hal_transmit_fn transmit;

    rf_hal_receive_start_fn receive_start;
    rf_hal_receive_stop_fn receive_stop;
    rf_hal_receive_read_fn receive_read;

    void *context;

} rf_hal_t;

typedef struct rf_driver rf_driver_t;

rf_driver_t *rf_driver_get_instance(void);

status_t rf_driver_init(
    rf_driver_t *driver,
    const rf_hal_t *hal);

status_t rf_driver_transmit(
    rf_driver_t *driver,
    const rf_pulse_t *pulses,
    uint32_t count);

status_t rf_driver_receive_start(
    rf_driver_t *driver);

status_t rf_driver_receive_stop(
    rf_driver_t *driver);

status_t rf_driver_receive_read(
    rf_driver_t *driver,
    rf_pulse_t *pulses,
    uint32_t max_count,
    uint32_t *count);

#endif