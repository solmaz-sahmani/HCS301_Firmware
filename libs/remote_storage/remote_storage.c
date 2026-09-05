#include <stdbool.h>

#include "remote_storage.h"

typedef struct
{
    bool used;
    remote_info_t info;

} remote_slot_t;

struct remote_storage
{
    remote_slot_t slots[REMOTE_STORAGE_MAX_REMOTES];
    uint32_t count;
};

static remote_storage_t storage_instance;


/**
 * @brief Get the storage instance.
 *
 * @return Storage instance.
 */
remote_storage_t *remote_storage_get_instance(void)
{
    return &storage_instance;
}


/**
 * @brief Initialize remote storage.
 *
 * @param storage Storage instance.
 *
 * @return Operation status.
 */
status_t remote_storage_init(remote_storage_t *storage)
{
    if (storage == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < REMOTE_STORAGE_MAX_REMOTES; i++)
    {
        storage->slots[i].used = false;
        storage->slots[i].info.serial = 0U;
        storage->slots[i].info.counter = 0U;
    }

    storage->count = 0U;

    return STATUS_OK;
}


/**
 * @brief Add a remote.
 *
 * @param storage Storage instance.
 * @param serial Remote serial number.
 * @param counter Remote counter.
 *
 * @return Operation status.
 */
status_t remote_storage_add(
    remote_storage_t *storage,
    uint32_t serial,
    uint32_t counter)
{
    if (storage == NULL || serial == 0U)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < REMOTE_STORAGE_MAX_REMOTES; i++)
    {
        if (storage->slots[i].used &&
            storage->slots[i].info.serial == serial)
        {
            return STATUS_ALREADY_EXISTS;
        }
    }

    for (uint32_t i = 0U; i < REMOTE_STORAGE_MAX_REMOTES; i++)
    {
        if (!storage->slots[i].used)
        {
            storage->slots[i].used = true;
            storage->slots[i].info.serial = serial;
            storage->slots[i].info.counter = counter;

            storage->count++;

            return STATUS_OK;
        }
    }

    return STATUS_FULL;
}


/**
 * @brief Remove a remote.
 *
 * @param storage Storage instance.
 * @param serial Remote serial number.
 *
 * @return Operation status.
 */
status_t remote_storage_remove(
    remote_storage_t *storage,
    uint32_t serial)
{
    if (storage == NULL || serial == 0U)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < REMOTE_STORAGE_MAX_REMOTES; i++)
    {
        if (storage->slots[i].used &&
            storage->slots[i].info.serial == serial)
        {
            storage->slots[i].used = false;
            storage->slots[i].info.serial = 0U;
            storage->slots[i].info.counter = 0U;

            storage->count--;

            return STATUS_OK;
        }
    }

    return STATUS_NOT_FOUND;
}


/**
 * @brief Find a remote.
 *
 * @param storage Storage instance.
 * @param serial Remote serial number.
 * @param info Output remote information.
 *
 * @return Operation status.
 */
status_t remote_storage_find(
    remote_storage_t *storage,
    uint32_t serial,
    remote_info_t *info)
{
    if (storage == NULL || info == NULL || serial == 0U)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < REMOTE_STORAGE_MAX_REMOTES; i++)
    {
        if (storage->slots[i].used &&
            storage->slots[i].info.serial == serial)
        {
            *info = storage->slots[i].info;

            return STATUS_OK;
        }
    }

    return STATUS_NOT_FOUND;
}


/**
 * @brief Get the number of stored remotes.
 *
 * @param storage Storage instance.
 *
 * @return Number of remotes.
 */
uint32_t remote_storage_count(
    remote_storage_t *storage)
{
    if (storage == NULL)
    {
        return 0U;
    }

    return storage->count;
}