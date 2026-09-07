#ifndef RF_ESP32_H
#define RF_ESP32_H

#include "err_core.h"
#include "rf_driver.h"

/**
 * @brief Initialize ESP32 RF backend.
 */
status_t rf_esp32_init(
    rf_hal_t *hal,
    int gpio_num);

#endif