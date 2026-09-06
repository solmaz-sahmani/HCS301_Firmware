#ifndef RF_HAL_ESP32_H
#define RF_HAL_ESP32_H


#include "esp_err.h"


esp_err_t rf_hal_init(void);


esp_err_t rf_hal_send(
        const uint8_t *data,
        uint16_t length);


#endif