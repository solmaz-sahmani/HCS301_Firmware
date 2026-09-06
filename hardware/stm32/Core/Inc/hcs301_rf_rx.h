#ifndef HCS301_RF_RX_H
#define HCS301_RF_RX_H

#include <stdbool.h>
#include <stdint.h>

#include "main.h"

#define HCS301_RF_RX_BITS             66U
#define HCS301_RF_RX_TE_US            400U
#define HCS301_RF_RX_TOLERANCE_US     120U
#define HCS301_RF_RX_HEADER_MIN_US    2800U
#define HCS301_RF_RX_HEADER_MAX_US    5200U
#define HCS301_RF_RX_GUARD_MIN_US     8000U

typedef struct
{
    uint8_t bits[HCS301_RF_RX_BITS];
    uint8_t bit_count;
    uint32_t previous_capture;
    uint32_t high_duration;
    bool have_previous_capture;
    bool have_high_duration;
    bool frame_ready;
    bool receiving;
} hcs301_rf_rx_t;

HAL_StatusTypeDef hcs301_rf_rx_init(hcs301_rf_rx_t *rx);

HAL_StatusTypeDef hcs301_rf_rx_start(
    hcs301_rf_rx_t *rx,
    TIM_HandleTypeDef *htim);

void hcs301_rf_rx_capture_edge(
    hcs301_rf_rx_t *rx,
    TIM_HandleTypeDef *htim);

bool hcs301_rf_rx_frame_ready(const hcs301_rf_rx_t *rx);

bool hcs301_rf_rx_read(
    hcs301_rf_rx_t *rx,
    uint8_t bits[HCS301_RF_RX_BITS]);

#endif
