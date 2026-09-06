#ifndef FS1000A_TX_H
#define FS1000A_TX_H


#include <stdint.h>

#include "esp_err.h"


/**
 * @brief Initialize FS1000A transmitter.
 *
 * @return ESP_OK on success.
 */
esp_err_t fs1000a_tx_init(void);


/**
 * @brief Send RF waveform.
 *
 * @param bits Frame bits.
 *
 * @return ESP_OK on success.
 */
esp_err_t fs1000a_tx_send(
        const uint8_t *bits,
        uint16_t length);


#endif