/**
 * @file keeloq_sw.c
 * @brief Software implementation of the KeeLoq block cipher (encrypt/decrypt).
 */

#include <stddef.h>
#include <stdint.h>

#include "keeloq_sw.h"

#define KEELOQ_ROUNDS 528U        /**< Number of Feistel-like rounds (KeeLoq standard) */
#define KEELOQ_NLF    0x3A5C742EU /**< Non-linear feedback function lookup table */
#define KEELOQ_KEY_BITS 64U       /**< Key length in bits */

/**
 * @brief Extract a single bit from a 32-bit value.
 *
 * @param value    Source value.
 * @param position Bit position (0 = LSB).
 *
 * @return The bit value (0 or 1).
 */
static uint32_t get_bit32(
    uint32_t value,
    uint32_t position)
{
    return (value >> position) & 1U;
}

/**
 * @brief Extract a single bit from a 64-bit value.
 *
 * @param value    Source value.
 * @param position Bit position (0 = LSB).
 *
 * @return The bit value (0 or 1).
 */
static uint32_t get_bit64(
    uint64_t value,
    uint32_t position)
{
    return (uint32_t)((value >> position) & 1ULL);
}

/**
 * @brief Evaluate the KeeLoq non-linear function (NLF) for five input taps.
 *
 * @param value Current 32-bit state.
 * @param a,b,c,d,e Bit positions used to build the NLF lookup index.
 *
 * @return The NLF output bit (0 or 1).
 */
static uint32_t get_nlf_bit(
    uint32_t value,
    uint32_t a,
    uint32_t b,
    uint32_t c,
    uint32_t d,
    uint32_t e)
{
    uint32_t index;

    index =
        get_bit32(value, a) |
        (get_bit32(value, b) << 1U) |
        (get_bit32(value, c) << 2U) |
        (get_bit32(value, d) << 3U) |
        (get_bit32(value, e) << 4U);

    return get_bit32(KEELOQ_NLF, index);
}

/**
 * @brief Encrypt a 32-bit block using the KeeLoq cipher.
 *
 * @param data      Plaintext 32-bit block.
 * @param key       64-bit key.
 * @param encrypted Output: encrypted 32-bit block.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
status_t keeloq_encrypt(
    uint32_t data,
    uint64_t key,
    uint32_t *encrypted)
{
    uint32_t x;

    if (encrypted == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    x = data;

    for (uint32_t round = 0U;
         round < KEELOQ_ROUNDS;
         round++)
    {
        uint32_t feedback;

        feedback =
            get_bit32(x, 0U) ^
            get_bit32(x, 16U) ^
            get_bit64(key, round & (KEELOQ_KEY_BITS - 1U)) ^
            get_nlf_bit(
                x,
                1U,
                9U,
                20U,
                26U,
                31U);

        x =
            (x >> 1U) |
            (feedback << 31U);
    }

    *encrypted = x;

    return STATUS_OK;
}

/**
 * @brief Decrypt a 32-bit block using the KeeLoq cipher.
 *
 * @param data      Ciphertext 32-bit block.
 * @param key       64-bit key.
 * @param decrypted Output: decrypted 32-bit block.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
status_t keeloq_decrypt(
    uint32_t data,
    uint64_t key,
    uint32_t *decrypted)
{
    uint32_t x;

    if (decrypted == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    x = data;

    for (uint32_t round = 0U;
         round < KEELOQ_ROUNDS;
         round++)
    {
        uint32_t feedback;
        uint32_t key_bit_index;

        /* Equivalent to (15 - round) mod 64, computed without relying on
         * unsigned underflow for readability. */
        key_bit_index =
            (15U + KEELOQ_ROUNDS - (round % KEELOQ_KEY_BITS)) &
            (KEELOQ_KEY_BITS - 1U);

        feedback =
            get_bit32(x, 31U) ^
            get_bit32(x, 15U) ^
            get_bit64(key, key_bit_index) ^
            get_nlf_bit(
                x,
                0U,
                8U,
                19U,
                25U,
                30U);

        x =
            (x << 1U) |
            feedback;
    }

    *decrypted = x;

    return STATUS_OK;
}