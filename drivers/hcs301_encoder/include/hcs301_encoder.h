#ifndef HCS301_ENCODER_H
#define HCS301_ENCODER_H

#include <stdint.h>

#include "err_core.h"
#include "rf_driver.h"

#define HCS301_TE_US          400U
#define HCS301_FRAME_BITS     66U

#define HCS301_PREAMBLE_TE   23U
#define HCS301_HEADER_TE     10U
#define HCS301_GUARD_TE      39U

#define HCS301_MAX_PULSES    180U

/**
 * @brief Encode HCS301 bits into RF pulses.
 */
status_t hcs301_encoder_encode(
    const uint8_t bits[HCS301_FRAME_BITS],
    rf_pulse_t pulses[HCS301_MAX_PULSES],
    uint32_t *pulse_count);

#endif