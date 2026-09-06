#ifndef REMOTE_STORAGE_H
#define REMOTE_STORAGE_H

#include <stdint.h>

#include "err_core.h"

#define REMOTE_STORAGE_MAX_REMOTES 8U

typedef struct remote_storage remote_storage_t;

/**
 * @brief Remote information.
 */
typedef struct
{
    uint32_t serial;
    uint32_t counter;

} remote_info_t;

/**
 * @brief Get the storage instance.
 *
 * @return Storage instance.
 */
remote_storage_t *remote_storage_get_instance(void);

/**
 * @brief Initialize remote storage.
 *
 * @param storage Storage instance.
 *
 * @return Operation status.
 */
status_t remote_storage_init(remote_storage_t *storage);

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
    uint32_t counter);

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
    uint32_t serial);

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
    remote_info_t *info);

/**
 * @brief Get the number of stored remotes.
 *
 * @param storage Storage instance.
 *
 * @return Number of remotes.
 */
uint32_t remote_storage_count(
    remote_storage_t *storage);

#endif