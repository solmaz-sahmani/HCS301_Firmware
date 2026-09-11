#ifndef HCS301_DECODER_H
#define HCS301_DECODER_H

#include <stdint.h>

#include "err_core.h"
#include "rf_driver.h"
#include "hcs301_encoder.h"

/**
 * @brief Decode HCS301 RF pulses into logical frame bits.
 *
 * The decoder converts:
 *
 * preamble + header + data + guard
 *
 * into:
 *
 * HCS301_FRAME_BITS logical bits.
 *
 * No encryption or protocol-field decoding is performed here.
 *
 * @param pulses Input RF pulse array.
 * @param pulse_count Number of valid pulses.
 * @param bits Output HCS301 bit array.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
status_t hcs301_decoder_decode(
    const rf_pulse_t pulses[],
    uint32_t pulse_count,
    uint8_t bits[HCS301_FRAME_BITS]);

#endif