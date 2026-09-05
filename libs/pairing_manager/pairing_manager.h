#ifndef PAIRING_MANAGER_H
#define PAIRING_MANAGER_H

#include <stdint.h>

#include "err_core.h"
#include "remote_storage.h"

typedef enum
{
    PAIRING_IDLE = 0,
    PAIRING_WAITING,
    PAIRING_SUCCESS

} pairing_state_t;

typedef struct pairing_manager pairing_manager_t;

/**
 * @brief Get the pairing manager instance.
 *
 * @return Pairing manager instance.
 */
pairing_manager_t *pairing_manager_get_instance(void);

/**
 * @brief Initialize pairing manager.
 *
 * @param manager Pairing manager instance.
 *
 * @return Operation status.
 */
status_t pairing_manager_init(
    pairing_manager_t *manager);

/**
 * @brief Start pairing process.
 *
 * @param manager Pairing manager instance.
 *
 * @return Operation status.
 */
status_t pairing_manager_start(
    pairing_manager_t *manager);

/**
 * @brief Process a received remote.
 *
 * @param manager Pairing manager instance.
 * @param remote Remote information.
 *
 * @return Operation status.
 */
status_t pairing_manager_process_remote(
    pairing_manager_t *manager,
    const remote_info_t *remote);

/**
 * @brief Process pairing manager.
 *
 * @param manager Pairing manager instance.
 *
 * @return Operation status.
 */
status_t pairing_manager_process(
    pairing_manager_t *manager);

/**
 * @brief Get current pairing state.
 *
 * @param manager Pairing manager instance.
 * @param state Output pairing state.
 *
 * @return Operation status.
 */
status_t pairing_manager_get_state(
    pairing_manager_t *manager,
    pairing_state_t *state);

#endif