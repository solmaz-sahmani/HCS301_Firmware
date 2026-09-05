#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <stdbool.h>

#include "err_core.h"

typedef enum
{
    BUTTON_PAIR = 0,
    BUTTON_DELETE,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,

    BUTTON_COUNT

} button_id_t;

typedef struct button_manager button_manager_t;

/**
 * @brief Get the button manager instance.
 *
 * @return Button manager instance.
 */
button_manager_t *button_manager_get_instance(void);

/**
 * @brief Initialize button manager.
 *
 * @param manager Button manager instance.
 *
 * @return Operation status.
 */
status_t button_manager_init(
    button_manager_t *manager);

/**
 * @brief Set button state.
 *
 * @param manager Button manager instance.
 * @param button Button identifier.
 * @param pressed Button state.
 *
 * @return Operation status.
 */
status_t button_manager_set_state(
    button_manager_t *manager,
    button_id_t button,
    bool pressed);

/**
 * @brief Check whether a button is pressed.
 *
 * @param manager Button manager instance.
 * @param button Button identifier.
 * @param pressed Output state.
 *
 * @return Operation status.
 */
status_t button_manager_is_pressed(
    button_manager_t *manager,
    button_id_t button,
    bool *pressed);

#endif