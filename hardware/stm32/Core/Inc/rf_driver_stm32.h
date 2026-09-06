#ifndef RF_DRIVER_STM32_H
#define RF_DRIVER_STM32_H

#include <stdbool.h>
#include <stdint.h>

#include "hcs301_rf_rx.h"

HAL_StatusTypeDef rf_driver_stm32_init(void);

bool rf_driver_stm32_receive(uint8_t bits[66]);

#endif
