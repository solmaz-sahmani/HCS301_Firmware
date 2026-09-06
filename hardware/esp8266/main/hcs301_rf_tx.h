#ifndef HCS301_RF_TX_H
#define HCS301_RF_TX_H

#include <stdint.h>
#include "esp_err.h"

#define HCS301_RF_TX_GPIO 17

esp_err_t hcs301_rf_tx_init(void);
esp_err_t hcs301_rf_tx_send(const uint8_t bits[66]);

#endif
