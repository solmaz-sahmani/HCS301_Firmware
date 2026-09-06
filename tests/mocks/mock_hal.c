#include "mock_hal.h"

#define MOCK_GPIO_COUNT 16

static bool gpio_states[MOCK_GPIO_COUNT];

void mock_hal_set_gpio(uint8_t pin, bool state)
{
    if (pin < MOCK_GPIO_COUNT)
    {
        gpio_states[pin] = state;
    }
}

bool hal_gpio_read(uint8_t pin)
{
    if (pin < MOCK_GPIO_COUNT)
    {
        return gpio_states[pin];
    }

    return false;
}