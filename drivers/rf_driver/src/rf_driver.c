#include <stddef.h>
#include <stdbool.h>

#include "rf_driver.h"

struct rf_driver
{
    bool initialized;
    rf_hal_t hal;
};

static rf_driver_t rf_driver_instance;

rf_driver_t *rf_driver_get_instance(void)
{
    return &rf_driver_instance;
}

status_t rf_driver_init(
    rf_driver_t *driver,
    const rf_hal_t *hal)
{
    if (driver == NULL || hal == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    driver->hal = *hal;
    driver->initialized = true;

    return STATUS_OK;
}

status_t rf_driver_transmit(
    rf_driver_t *driver,
    const rf_pulse_t *pulses,
    uint32_t count)
{
    if (driver == NULL ||
        pulses == NULL ||
        count == 0U)
    {
        return STATUS_INVALID_ARG;
    }

    if (!driver->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (driver->hal.transmit == NULL)
    {
        return STATUS_NOT_INITIALIZED;
    }

    return driver->hal.transmit(
        driver->hal.context,
        pulses,
        count);
}

status_t rf_driver_receive_start(
    rf_driver_t *driver)
{
    if (driver == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (!driver->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (driver->hal.receive_start == NULL)
    {
        return STATUS_NOT_INITIALIZED;
    }

    return driver->hal.receive_start(
        driver->hal.context);
}

status_t rf_driver_receive_stop(
    rf_driver_t *driver)
{
    if (driver == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (!driver->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (driver->hal.receive_stop == NULL)
    {
        return STATUS_NOT_INITIALIZED;
    }

    return driver->hal.receive_stop(
        driver->hal.context);
}

status_t rf_driver_receive_read(
    rf_driver_t *driver,
    rf_pulse_t *pulses,
    uint32_t max_count,
    uint32_t *count)
{
    if (driver == NULL ||
        pulses == NULL ||
        count == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (driver->hal.receive_read == NULL)
    {
        return STATUS_ERROR;
    }

    return
        driver->hal.receive_read(
            driver->hal.context,
            pulses,
            max_count,
            count);
}