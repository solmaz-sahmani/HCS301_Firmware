#include <stddef.h>
#include <stdint.h>

#include "button_manager.h"

struct button_manager
{
    bool states[BUTTON_COUNT];
};

static button_manager_t button_manager_instance;

/**
 * @brief Get the button manager instance.
 *
 * @return Button manager instance.
 */
button_manager_t *button_manager_get_instance(void)
{
    return &button_manager_instance;
}

/**
 * @brief Initialize button manager.
 *
 * @param manager Button manager instance.
 *
 * @return Operation status.
 */
status_t button_manager_init(
    button_manager_t *manager)
{
    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < BUTTON_COUNT; i++)
    {
        manager->states[i] = false;
    }

    return STATUS_OK;
}

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
    bool pressed)
{
    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (button >= BUTTON_COUNT)
    {
        return STATUS_INVALID_ARG;
    }

    manager->states[button] = pressed;

    return STATUS_OK;
}

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
    bool *pressed)
{
    if (manager == NULL || pressed == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (button >= BUTTON_COUNT)
    {
        return STATUS_INVALID_ARG;
    }

    *pressed = manager->states[button];

    return STATUS_OK;
}