#ifndef MOCK_HAL_H
#define MOCK_HAL_H

#include <stdbool.h>
#include <stdint.h>

void mock_hal_set_gpio(uint8_t pin, bool state);

#endif