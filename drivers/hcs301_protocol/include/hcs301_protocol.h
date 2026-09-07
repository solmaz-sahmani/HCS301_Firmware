#ifndef HCS301_PROTOCOL_H
#define HCS301_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

#include "err_core.h"

/**
 * @brief Number of bits in an HCS301 frame.
 */
#define HCS301_FRAME_BITS 66U

/**
 * @brief Number of encrypted bits.
 */
#define HCS301_ENCRYPTED_BITS 32U

/**
 * @brief Number of fixed bits.
 */
#define HCS301_FIXED_BITS 34U

/**
 * @brief HCS301 decoded frame.
 */
typedef struct
{
    uint32_t encrypted;
    uint32_t serial;
    uint16_t counter;
    uint16_t discrimination;
    uint8_t button_status;
    uint8_t overflow;
    bool repeat;
    bool vlow;

} hcs301_frame_t;

/**
 * @brief HCS301 protocol decoder.
 */
typedef struct hcs301_protocol hcs301_protocol_t;

/**
 * @brief Get the HCS301 protocol instance.
 *
 * @return Protocol instance.
 */
hcs301_protocol_t *hcs301_protocol_get_instance(void);

/**
 * @brief Initialize HCS301 protocol.
 *
 * @param protocol Protocol instance.
 *
 * @return Operation status.
 */
status_t hcs301_protocol_init(
    hcs301_protocol_t *protocol);

/**
 * @brief Encode an HCS301 frame.
 *
 * @param protocol Protocol instance.
 * @param frame Frame information.
 * @param key Encryption key.
 * @param bits Output 66-bit frame.
 *
 * @return Operation status.
 */
status_t hcs301_protocol_encode(
    hcs301_protocol_t *protocol,
    const hcs301_frame_t *frame,
    uint64_t key,
    uint8_t bits[HCS301_FRAME_BITS]);

/**
 * @brief Decode an HCS301 frame.
 *
 * @param protocol Protocol instance.
 * @param bits Input 66-bit frame.
 * @param key Encryption key.
 * @param frame Output decoded frame.
 *
 * @return Operation status.
 */
status_t hcs301_protocol_decode(
    hcs301_protocol_t *protocol,
    const uint8_t bits[HCS301_FRAME_BITS],
    uint64_t key,
    hcs301_frame_t *frame);

/**
 * @brief Verify decoded frame.
 *
 * @param protocol Protocol instance.
 * @param frame Decoded frame.
 * @param serial Expected serial number.
 * @param discrimination Expected discrimination value.
 *
 * @return Operation status.
 */
status_t hcs301_protocol_verify(
    hcs301_protocol_t *protocol,
    const hcs301_frame_t *frame,
    uint32_t serial,
    uint16_t discrimination);

#endif