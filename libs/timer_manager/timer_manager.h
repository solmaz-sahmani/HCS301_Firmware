#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "err_core.h"

typedef uint32_t timer_id_t;

#define TIMER_INVALID_ID 0U

typedef struct timer_manager timer_manager_t;

/**
 * @brief Get the timer manager instance.
 *
 * @return Timer manager instance.
 */
timer_manager_t *timer_manager_get_instance(void);

/**
 * @brief Initialize timer manager.
 *
 * @param manager Timer manager instance.
 *
 * @return Operation status.
 */
status_t timer_manager_init(
    timer_manager_t *manager);

/**
 * @brief Start a timer.
 *
 * @param manager Timer manager instance.
 * @param duration_ms Timer duration in milliseconds.
 * @param timer_id Output timer identifier.
 *
 * @return Operation status.
 */
status_t timer_manager_start(
    timer_manager_t *manager,
    uint32_t duration_ms,
    timer_id_t *timer_id);

/**
 * @brief Check whether a timer has expired.
 *
 * @param manager Timer manager instance.
 * @param timer_id Timer identifier.
 * @param expired Output expiration state.
 *
 * @return Operation status.
 */
status_t timer_manager_is_expired(
    timer_manager_t *manager,
    timer_id_t timer_id,
    bool *expired);

/**
 * @brief Stop a timer.
 *
 * @param manager Timer manager instance.
 * @param timer_id Timer identifier.
 *
 * @return Operation status.
 */
status_t timer_manager_stop(
    timer_manager_t *manager,
    timer_id_t timer_id);

/**
 * @brief Advance timer manager time.
 *
 * @param manager Timer manager instance.
 * @param elapsed_ms Elapsed time in milliseconds.
 *
 * @return Operation status.
 */
status_t timer_manager_update(
    timer_manager_t *manager,
    uint32_t elapsed_ms);

#endif