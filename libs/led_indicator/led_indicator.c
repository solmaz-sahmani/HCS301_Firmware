#include <stddef.h>
#include <stdint.h>

#include "led_indicator.h"

struct led_indicator
{
    bool states[LED_COUNT];
};

static led_indicator_t led_indicator_instance;

/**
 * @brief Get the LED indicator instance.
 *
 * @return LED indicator instance.
 */
led_indicator_t *led_indicator_get_instance(void)
{
    return &led_indicator_instance;
}

/**
 * @brief Initialize LED indicator.
 *
 * @param indicator LED indicator instance.
 *
 * @return Operation status.
 */
status_t led_indicator_init(
    led_indicator_t *indicator)
{
    if (indicator == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < LED_COUNT; i++)
    {
        indicator->states[i] = false;
    }

    return STATUS_OK;
}

/**
 * @brief Turn on an LED.
 *
 * @param indicator LED indicator instance.
 * @param led LED identifier.
 *
 * @return Operation status.
 */
status_t led_indicator_on(
    led_indicator_t *indicator,
    led_id_t led)
{
    if (indicator == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (led >= LED_COUNT)
    {
        return STATUS_INVALID_ARG;
    }

    indicator->states[led] = true;

    return STATUS_OK;
}

/**
 * @brief Turn off an LED.
 *
 * @param indicator LED indicator instance.
 * @param led LED identifier.
 *
 * @return Operation status.
 */
status_t led_indicator_off(
    led_indicator_t *indicator,
    led_id_t led)
{
    if (indicator == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (led >= LED_COUNT)
    {
        return STATUS_INVALID_ARG;
    }

    indicator->states[led] = false;

    return STATUS_OK;
}

/**
 * @brief Get LED state.
 *
 * @param indicator LED indicator instance.
 * @param led LED identifier.
 * @param on Output LED state.
 *
 * @return Operation status.
 */
status_t led_indicator_is_on(
    led_indicator_t *indicator,
    led_id_t led,
    bool *on)
{
    if (indicator == NULL || on == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (led >= LED_COUNT)
    {
        return STATUS_INVALID_ARG;
    }

    *on = indicator->states[led];

    return STATUS_OK;
}

/**
 * @brief Turn all LEDs on.
 *
 * @param indicator LED indicator instance.
 *
 * @return Operation status.
 */
status_t led_indicator_all_on(
    led_indicator_t *indicator)
{
    if (indicator == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < LED_COUNT; i++)
    {
        indicator->states[i] = true;
    }

    return STATUS_OK;
}

/**
 * @brief Turn all LEDs off.
 *
 * @param indicator LED indicator instance.
 *
 * @return Operation status.
 */
status_t led_indicator_all_off(
    led_indicator_t *indicator)
{
    if (indicator == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < LED_COUNT; i++)
    {
        indicator->states[i] = false;
    }

    return STATUS_OK;
}