#include <stddef.h>

#include "power_manager.h"

struct power_manager
{
    bool initialized;
    bool sleeping;

    power_hal_t hal;
};

static power_manager_t power_manager_instance;

power_manager_t *power_manager_get_instance(void)
{
    return &power_manager_instance;
}

status_t power_manager_init(
    power_manager_t *manager,
    const power_hal_t *hal)
{
    if (manager == NULL || hal == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (hal->sleep == NULL || hal->wake == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    manager->hal = *hal;

    manager->initialized = true;
    manager->sleeping = false;

    return STATUS_OK;
}

status_t power_manager_sleep(
    power_manager_t *manager)
{
    status_t status;

    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (!manager->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (manager->sleeping)
    {
        return STATUS_OK;
    }

    status = manager->hal.sleep(
        manager->hal.context);

    if (status != STATUS_OK)
    {
        return status;
    }

    manager->sleeping = true;

    return STATUS_OK;
}

status_t power_manager_wake(
    power_manager_t *manager)
{
    status_t status;

    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (!manager->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (!manager->sleeping)
    {
        return STATUS_OK;
    }

    status = manager->hal.wake(
        manager->hal.context);

    if (status != STATUS_OK)
    {
        return status;
    }

    manager->sleeping = false;

    return STATUS_OK;
}

status_t power_manager_is_sleeping(
    power_manager_t *manager,
    bool *sleeping)
{
    if (manager == NULL || sleeping == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (!manager->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    *sleeping = manager->sleeping;

    return STATUS_OK;
}