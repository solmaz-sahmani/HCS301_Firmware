#include <stddef.h>

#include "delete_manager.h"

struct delete_manager
{
    delete_state_t state;
};

static delete_manager_t delete_manager_instance;

delete_manager_t *delete_manager_get_instance(void)
{
    return &delete_manager_instance;
}

status_t delete_manager_init(delete_manager_t *manager)
{
    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    manager->state = DELETE_IDLE;

    return STATUS_OK;
}

status_t delete_manager_start(delete_manager_t *manager)
{
    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (manager->state != DELETE_IDLE)
    {
        return STATUS_ERROR;
    }

    manager->state = DELETE_WAITING;

    return STATUS_OK;
}

status_t delete_manager_process_remote(
    delete_manager_t *manager,
    uint32_t serial)
{
    status_t status;

    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (manager->state != DELETE_WAITING)
    {
        return STATUS_ERROR;
    }

    status = remote_storage_remove(
        remote_storage_get_instance(),
        serial);

    if (status != STATUS_OK)
    {
        return status;
    }

    manager->state = DELETE_SUCCESS;

    return STATUS_OK;
}

status_t delete_manager_get_state(
    delete_manager_t *manager,
    delete_state_t *state)
{
    if (manager == NULL || state == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    *state = manager->state;

    return STATUS_OK;
}