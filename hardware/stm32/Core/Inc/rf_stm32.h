#ifndef RF_STM32_H
#define RF_STM32_H

#include "stm32f1xx_hal.h"

#include "err_core.h"
#include "rf_driver.h"

#define RF_STM32_MAX_PULSES 200U

typedef struct
{
    TIM_HandleTypeDef *timer;
    uint32_t channel;

    GPIO_TypeDef *data_port;
    uint16_t data_pin;

} rf_stm32_config_t;

status_t rf_stm32_init(
    rf_hal_t *hal,
    const rf_stm32_config_t *config);

#endif