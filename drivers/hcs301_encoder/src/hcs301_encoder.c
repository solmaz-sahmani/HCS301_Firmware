/**
 * @file hcs301_encoder.c
 * @brief Pulse-train encoder for the HCS301 (KeeLoq-style) RF protocol.
 */

#include <stddef.h>
#include <stdbool.h>

#include "hcs301_encoder.h"

/**
 * @brief Append a single pulse to the pulse buffer.
 *
 * @param pulses   Destination pulse array.
 * @param count    In/out pulse count.
 * @param level    Pulse level (true = HIGH, false = LOW).
 * @param duration_us Pulse duration in microseconds.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
static status_t add_pulse(
    rf_pulse_t pulses[],
    uint32_t *count,
    bool level,
    uint16_t duration_us)
{
    if (pulses == NULL || count == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (*count >= HCS301_MAX_PULSES)
    {
        return STATUS_FULL;
    }

    pulses[*count].level = level;
    pulses[*count].duration_us = duration_us;

    (*count)++;

    return STATUS_OK;
}

/**
 * @brief Append a pulse whose duration is a multiple of the protocol's
 *        base time element (TE).
 *
 * @param pulses   Destination pulse array.
 * @param count    In/out pulse count.
 * @param level    Pulse level (true = HIGH, false = LOW).
 * @param te_count Duration expressed in TE units.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
static status_t add_te(
    rf_pulse_t pulses[],
    uint32_t *count,
    bool level,
    uint32_t te_count)
{
    uint32_t duration_us =
        te_count * HCS301_TE_US;

    /* rf_pulse_t.duration_us is ultimately packed into a 15-bit RMT field
     * (max 32767). Reject anything that would silently truncate. */
    if (duration_us > 0x7FFFU)
    {
        return STATUS_INVALID_ARG;
    }

    return add_pulse(
        pulses,
        count,
        level,
        (uint16_t)duration_us);
}

/**
 * @brief Append the alternating HIGH/LOW preamble pulses.
 *
 * @param pulses Destination pulse array.
 * @param count  In/out pulse count.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
static status_t add_preamble(
    rf_pulse_t pulses[],
    uint32_t *count)
{
    bool level = true;

    for (uint32_t i = 0U;
         i < HCS301_PREAMBLE_TE;
         i++)
    {
        status_t status = add_te(
            pulses,
            count,
            level,
            1U);

        if (status != STATUS_OK)
        {
            return status;
        }

        level = !level;
    }

    return STATUS_OK;
}

/**
 * @brief Append the sync/header LOW pulse that follows the preamble.
 *
 * @param pulses Destination pulse array.
 * @param count  In/out pulse count.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
static status_t add_header(
    rf_pulse_t pulses[],
    uint32_t *count)
{
    return add_te(
        pulses,
        count,
        false,
        HCS301_HEADER_TE);
}

/**
 * @brief Append the two pulses (HIGH+LOW) that encode a single data bit.
 *
 * Logic 0 = 1 TE high + 2 TE low.
 * Logic 1 = 2 TE high + 1 TE low.
 *
 * @param pulses Destination pulse array.
 * @param count  In/out pulse count.
 * @param bit    Bit value to encode (0 or 1).
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
static status_t add_bit(
    rf_pulse_t pulses[],
    uint32_t *count,
    uint8_t bit)
{
    if (bit == 0U)
    {
        status_t status = add_te(
            pulses,
            count,
            true,
            1U);

        if (status != STATUS_OK)
        {
            return status;
        }

        return add_te(
            pulses,
            count,
            false,
            2U);
    }

    status_t status = add_te(
        pulses,
        count,
        true,
        2U);

    if (status != STATUS_OK)
    {
        return status;
    }

    return add_te(
        pulses,
        count,
        false,
        1U);
}

/**
 * @brief Append all data bits of the frame.
 *
 * @param bits   Array of HCS301_FRAME_BITS bit values (0 or 1).
 * @param pulses Destination pulse array.
 * @param count  In/out pulse count.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
static status_t add_data(
    const uint8_t bits[HCS301_FRAME_BITS],
    rf_pulse_t pulses[],
    uint32_t *count)
{
    for (uint32_t i = 0U;
         i < HCS301_FRAME_BITS;
         i++)
    {
        status_t status = add_bit(
            pulses,
            count,
            bits[i]);

        if (status != STATUS_OK)
        {
            return status;
        }
    }

    return STATUS_OK;
}

/**
 * @brief Append the trailing guard-time LOW pulse.
 *
 * @param pulses Destination pulse array.
 * @param count  In/out pulse count.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
static status_t add_guard(
    rf_pulse_t pulses[],
    uint32_t *count)
{
    return add_te(
        pulses,
        count,
        false,
        HCS301_GUARD_TE);
}

/**
 * @brief Encode an HCS301 frame (preamble + header + data bits + guard)
 *        into an rf_pulse_t array ready for transmission.
 *
 * @param bits        Array of HCS301_FRAME_BITS bit values (0 or 1).
 * @param pulses      Output pulse array, must hold at least HCS301_MAX_PULSES entries.
 * @param pulse_count Output: number of pulses written to @p pulses.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
status_t hcs301_encoder_encode(
    const uint8_t bits[HCS301_FRAME_BITS],
    rf_pulse_t pulses[HCS301_MAX_PULSES],
    uint32_t *pulse_count)
{
    if (bits == NULL ||
        pulses == NULL ||
        pulse_count == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    uint32_t count = 0U;

    status_t status;

    status = add_preamble(
        pulses,
        &count);

    if (status != STATUS_OK)
    {
        return status;
    }

    status = add_header(
        pulses,
        &count);

    if (status != STATUS_OK)
    {
        return status;
    }

    status = add_data(
        bits,
        pulses,
        &count);

    if (status != STATUS_OK)
    {
        return status;
    }

    status = add_guard(
        pulses,
        &count);

    if (status != STATUS_OK)
    {
        return status;
    }

    *pulse_count = count;

    return STATUS_OK;
}