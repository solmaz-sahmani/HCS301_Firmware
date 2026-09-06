#ifndef RF_DRIVER_ESP32_H
#define RF_DRIVER_ESP32_H

#include <stdint.h>
#include "esp_err.h"

esp_err_t rf_driver_esp32_init(void);
esp_err_t rf_driver_esp32_transmit_frame(const uint8_t bits[66]);

#endif
