#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <stdbool.h>

#include "err_core.h"

/**
 * @brief Power HAL sleep callback.
 *
 * @param context HAL context.
 *
 * @return Operation status.
 */
typedef status_t (*power_hal_sleep_fn)(void *context);

/**
 * @brief Power HAL wake callback.
 *
 * @param context HAL context.
 *
 * @return Operation status.
 */
typedef status_t (*power_hal_wake_fn)(void *context);

/**
 * @brief Power HAL interface.
 */
typedef struct
{
    power_hal_sleep_fn sleep;
    power_hal_wake_fn wake;
    void *context;

} power_hal_t;

typedef struct power_manager power_manager_t;

/**
 * @brief Get the power manager instance.
 *
 * @return Power manager instance.
 */
power_manager_t *power_manager_get_instance(void);

/**
 * @brief Initialize power manager.
 *
 * @param manager Power manager instance.
 * @param hal Power HAL interface.
 *
 * @return Operation status.
 */
status_t power_manager_init(
    power_manager_t *manager,
    const power_hal_t *hal);

/**
 * @brief Enter low power mode.
 *
 * @param manager Power manager instance.
 *
 * @return Operation status.
 */
status_t power_manager_sleep(
    power_manager_t *manager);

/**
 * @brief Wake the system.
 *
 * @param manager Power manager instance.
 *
 * @return Operation status.
 */
status_t power_manager_wake(
    power_manager_t *manager);

/**
 * @brief Check whether the system is sleeping.
 *
 * @param manager Power manager instance.
 * @param sleeping Output sleep state.
 *
 * @return Operation status.
 */
status_t power_manager_is_sleeping(
    power_manager_t *manager,
    bool *sleeping);

#endif