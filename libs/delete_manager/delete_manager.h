#ifndef DELETE_MANAGER_H
#define DELETE_MANAGER_H

#include <stdint.h>

#include "err_core.h"
#include "remote_storage.h"

typedef enum
{
    DELETE_IDLE = 0,
    DELETE_WAITING,
    DELETE_SUCCESS

} delete_state_t;

typedef struct delete_manager delete_manager_t;

/**
 * @brief Get the delete manager instance.
 *
 * @return Delete manager instance.
 */
delete_manager_t *delete_manager_get_instance(void);

/**
 * @brief Initialize delete manager.
 *
 * @param manager Delete manager instance.
 *
 * @return Operation status.
 */
status_t delete_manager_init(
    delete_manager_t *manager);

/**
 * @brief Start delete process.
 *
 * @param manager Delete manager instance.
 *
 * @return Operation status.
 */
status_t delete_manager_start(
    delete_manager_t *manager);

/**
 * @brief Process a remote deletion.
 *
 * @param manager Delete manager instance.
 * @param serial Remote serial number.
 *
 * @return Operation status.
 */
status_t delete_manager_process_remote(
    delete_manager_t *manager,
    uint32_t serial);

/**
 * @brief Get current delete state.
 *
 * @param manager Delete manager instance.
 * @param state Output delete state.
 *
 * @return Operation status.
 */
status_t delete_manager_get_state(
    delete_manager_t *manager,
    delete_state_t *state);

#endif