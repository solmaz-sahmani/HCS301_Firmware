#include <stddef.h>
#include <stdint.h>

#include "keeloq_sw.h"

#define KEELOQ_ROUNDS 528U
#define KEELOQ_NLF    0x3A5C742EU

static uint32_t get_bit32(
    uint32_t value,
    uint32_t position)
{
    return (value >> position) & 1U;
}

static uint32_t get_bit64(
    uint64_t value,
    uint32_t position)
{
    return (uint32_t)((value >> position) & 1ULL);
}

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
            get_bit64(key, round & 63U) ^
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

        feedback =
            get_bit32(x, 31U) ^
            get_bit32(x, 15U) ^
            get_bit64(
                key,
                (15U - round) & 63U) ^
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