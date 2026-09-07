#include <stddef.h>
#include <stdbool.h>

#include "rf_driver.h"

#define RF_MAX_PULSES 128U

struct rf_driver
{
    bool initialized;

    rf_pulse_t received_pulses[RF_MAX_PULSES];
    uint32_t received_count;
};

static rf_driver_t rf_driver_instance;

rf_driver_t *rf_driver_get_instance(void)
{
    return &rf_driver_instance;
}

status_t rf_driver_init(
    rf_driver_t *driver)
{
    if (driver == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    driver->initialized = true;
    driver->received_count = 0U;

    return STATUS_OK;
}

status_t rf_driver_transmit(
    rf_driver_t *driver,
    const rf_pulse_t *pulses,
    uint32_t count)
{
    if (driver == NULL || pulses == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (!driver->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (count == 0U)
    {
        return STATUS_INVALID_ARG;
    }

    if (count > RF_MAX_PULSES)
    {
        return STATUS_FULL;
    }

    for (uint32_t i = 0U; i < count; i++)
    {
        driver->received_pulses[i] = pulses[i];
    }

    driver->received_count = count;

    return STATUS_OK;
}

status_t rf_driver_receive(
    rf_driver_t *driver,
    rf_pulse_t *pulses,
    uint32_t count,
    uint32_t *received)
{
    if (driver == NULL ||
        pulses == NULL ||
        received == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (!driver->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (count == 0U || count > RF_MAX_PULSES)
    {
        return STATUS_INVALID_ARG;
    }

    if (driver->received_count > count)
    {
        return STATUS_FULL;
    }

    for (uint32_t i = 0U;
         i < driver->received_count;
         i++)
    {
        pulses[i] =
            driver->received_pulses[i];
    }

    *received = driver->received_count;

    return STATUS_OK;
}