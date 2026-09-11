#include <stddef.h>

#include "hcs301_protocol.h"
#include "keeloq_sw.h"


#define HCS301_SERIAL_MASK        0x0FFFFFFFU
#define HCS301_BUTTON_MASK        0x0FU
#define HCS301_DISC_MASK          0x03FFU
#define HCS301_OVR_MASK           0x03U

#define HCS301_BUTTON_SHIFT       28U
#define HCS301_OVR_SHIFT          26U
#define HCS301_DISC_SHIFT         16U


struct hcs301_protocol
{
    bool initialized;
};


static hcs301_protocol_t hcs301_protocol_instance;


/**
 * @brief Write value bits into bit array.
 */
static void put_bits(
    uint8_t *bits,
    uint32_t offset,
    uint32_t value,
    uint32_t count)
{
    for (uint32_t i = 0U; i < count; i++)
    {
        bits[offset + i] =
            (uint8_t)((value >> i) & 1U);
    }
}


/**
 * @brief Read value from bit array.
 */
static uint32_t get_bits(
    const uint8_t *bits,
    uint32_t offset,
    uint32_t count)
{
    uint32_t value = 0U;

    for (uint32_t i = 0U; i < count; i++)
    {
        value |=
            ((uint32_t)bits[offset + i] << i);
    }

    return value;
}


/**
 * @brief Get HCS301 protocol singleton.
 */
hcs301_protocol_t *hcs301_protocol_get_instance(void)
{
    return &hcs301_protocol_instance;
}


/**
 * @brief Initialize HCS301 protocol.
 */
status_t hcs301_protocol_init(
    hcs301_protocol_t *protocol)
{
    if (protocol == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    protocol->initialized = true;

    return STATUS_OK;
}


/**
 * @brief Encode HCS301 frame.
 */
status_t hcs301_protocol_encode(
    hcs301_protocol_t *protocol,
    const hcs301_frame_t *frame,
    uint64_t key,
    uint8_t bits[HCS301_FRAME_BITS])
{
    uint32_t plaintext;
    uint32_t encrypted;


    if (protocol == NULL ||
        frame == NULL ||
        bits == NULL)
    {
        return STATUS_INVALID_ARG;
    }


    if (!protocol->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }


    if ((frame->serial & ~HCS301_SERIAL_MASK) != 0U)
    {
        return STATUS_INVALID_ARG;
    }


    if (frame->button_status > HCS301_BUTTON_MASK)
    {
        return STATUS_INVALID_ARG;
    }


    if (frame->discrimination > HCS301_DISC_MASK)
    {
        return STATUS_INVALID_ARG;
    }


    if (frame->overflow > HCS301_OVR_MASK)
    {
        return STATUS_INVALID_ARG;
    }


    /*
     * Construct 32-bit KeeLoq plaintext.
     *
     *     Counter          : bits 0..15
     *     Discrimination   : bits 16..25
     *     Overflow         : bits 26..27
     *     Button           : bits 28..31
     */
    plaintext =
        ((uint32_t)frame->counter) |
        ((uint32_t)frame->discrimination
            << HCS301_DISC_SHIFT) |
        ((uint32_t)frame->overflow
            << HCS301_OVR_SHIFT) |
        ((uint32_t)frame->button_status
            << HCS301_BUTTON_SHIFT);


    /*
     * Encrypt rolling-code portion.
     */
    if (keeloq_encrypt(
            plaintext,
            key,
            &encrypted) != STATUS_OK)
    {
        return STATUS_ERROR;
    }


    /*
     * Clear complete frame.
     */
    for (uint32_t i = 0U;
         i < HCS301_FRAME_BITS;
         i++)
    {
        bits[i] = 0U;
    }


    /*
     * Encrypted portion:
     *
     * bits 0..31
     */
    put_bits(
        bits,
        0U,
        encrypted,
        HCS301_ENCRYPTED_BITS);


    /*
     * Fixed portion:
     *
     * Serial:
     * bits 32..59
     */
    put_bits(
        bits,
        32U,
        frame->serial,
        28U);


    /*
     * Button:
     *
     * bits 60..63
     */
    put_bits(
        bits,
        60U,
        frame->button_status,
        4U);


    /*
     * VLOW:
     *
     * bit 64
     */
    bits[64U] =
        frame->vlow ? 1U : 0U;


    /*
     * Repeat:
     *
     * bit 65
     */
    bits[65U] =
        frame->repeat ? 1U : 0U;


    return STATUS_OK;
}


/**
 * @brief Decode HCS301 frame.
 */
status_t hcs301_protocol_decode(
    hcs301_protocol_t *protocol,
    const uint8_t bits[HCS301_FRAME_BITS],
    uint64_t key,
    hcs301_frame_t *frame)
{
    uint32_t encrypted;
    uint32_t plaintext;


    if (protocol == NULL ||
        bits == NULL ||
        frame == NULL)
    {
        return STATUS_INVALID_ARG;
    }


    if (!protocol->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }


    /*
     * Extract encrypted 32-bit portion.
     */
    encrypted =
        get_bits(
            bits,
            0U,
            HCS301_ENCRYPTED_BITS);


    /*
     * Decrypt KeeLoq portion.
     */
    if (keeloq_decrypt(
            encrypted,
            key,
            &plaintext) != STATUS_OK)
    {
        return STATUS_ERROR;
    }


    /*
     * Store encrypted value.
     */
    frame->encrypted = encrypted;


    /*
     * Decode rolling counter.
     */
    frame->counter =
        (uint16_t)(
            plaintext & 0xFFFFU);


    /*
     * Decode discrimination.
     */
    frame->discrimination =
        (uint16_t)(
            (plaintext >> HCS301_DISC_SHIFT) &
            HCS301_DISC_MASK);


    /*
     * Decode overflow.
     */
    frame->overflow =
        (uint8_t)(
            (plaintext >> HCS301_OVR_SHIFT) &
            HCS301_OVR_MASK);


    /*
     * Decode button status.
     */
    frame->button_status =
        (uint8_t)(
            (plaintext >> HCS301_BUTTON_SHIFT) &
            HCS301_BUTTON_MASK);


    /*
     * Decode serial number.
     */
    frame->serial =
        get_bits(
            bits,
            32U,
            28U);


    /*
     * Decode VLOW.
     */
    frame->vlow =
        bits[64U] != 0U;


    /*
     * Decode repeat flag.
     */
    frame->repeat =
        bits[65U] != 0U;


    return STATUS_OK;
}


/**
 * @brief Verify decoded HCS301 frame.
 */
status_t hcs301_protocol_verify(
    hcs301_protocol_t *protocol,
    const hcs301_frame_t *frame,
    uint32_t serial,
    uint16_t discrimination)
{
    if (protocol == NULL ||
        frame == NULL)
    {
        return STATUS_INVALID_ARG;
    }


    if (!protocol->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }


    if ((serial & ~HCS301_SERIAL_MASK) != 0U)
    {
        return STATUS_INVALID_ARG;
    }


    if (discrimination > HCS301_DISC_MASK)
    {
        return STATUS_INVALID_ARG;
    }


    /*
     * Verify serial number.
     */
    if (frame->serial != serial)
    {
        return STATUS_ERROR;
    }


    /*
     * Verify discrimination value.
     */
    if (frame->discrimination != discrimination)
    {
        return STATUS_ERROR;
    }


    return STATUS_OK;
}