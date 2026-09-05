#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include <stdbool.h>

#include "err_core.h"

typedef enum
{
    LED_1 = 0,
    LED_2,
    LED_3,
    LED_4,

    LED_COUNT

} led_id_t;

typedef struct led_indicator led_indicator_t;

/**
 * @brief Get the LED indicator instance.
 *
 * @return LED indicator instance.
 */
led_indicator_t *led_indicator_get_instance(void);

/**
 * @brief Initialize LED indicator.
 *
 * @param indicator LED indicator instance.
 *
 * @return Operation status.
 */
status_t led_indicator_init(
    led_indicator_t *indicator);

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
    led_id_t led);

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
    led_id_t led);

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
    bool *on);

/**
 * @brief Turn all LEDs on.
 *
 * @param indicator LED indicator instance.
 *
 * @return Operation status.
 */
status_t led_indicator_all_on(
    led_indicator_t *indicator);

/**
 * @brief Turn all LEDs off.
 *
 * @param indicator LED indicator instance.
 *
 * @return Operation status.
 */
status_t led_indicator_all_off(
    led_indicator_t *indicator);

#endif