/**
 * @file hcs301_decoder.c
 *
 * @brief Pulse-train decoder for the HCS301 RF protocol.
 */

#include <stddef.h>
#include <stdbool.h>

#include "hcs301_decoder.h"

/*
 * RF timing tolerance.
 *
 * TE = 400 us.
 *
 * A 25% tolerance allows:
 *
 * 1 TE -> 300..500 us
 * 2 TE -> 600..1000 us
 *
 * This is suitable for the first software implementation.
 */
#define HCS301_TIMING_TOLERANCE_PERCENT 25U

/**
 * @brief Check whether a measured duration matches an expected duration.
 *
 * @param actual Actual pulse duration.
 * @param expected Expected pulse duration.
 *
 * @return true when the duration is inside the allowed tolerance.
 */
static bool duration_matches(
    uint32_t actual,
    uint32_t expected)
{
    uint32_t tolerance;
    uint32_t minimum;
    uint32_t maximum;

    tolerance =
        (expected * HCS301_TIMING_TOLERANCE_PERCENT) / 100U;

    minimum = expected - tolerance;
    maximum = expected + tolerance;

    return (actual >= minimum && actual <= maximum);
}

/**
 * @brief Check whether a pulse has the expected level and duration.
 *
 * @param pulse RF pulse.
 * @param level Expected level.
 * @param duration Expected duration.
 *
 * @return true when the pulse matches.
 */
static bool pulse_matches(
    const rf_pulse_t *pulse,
    bool level,
    uint32_t duration)
{
    if (pulse == NULL)
    {
        return false;
    }

    if (pulse->level != level)
    {
        return false;
    }

    return duration_matches(
        pulse->duration_us,
        duration);
}

/**
 * @brief Find the beginning of an HCS301 frame.
 *
 * The encoder produces:
 *
 * 23 alternating 1-TE pulses
 * followed by
 * one LOW 10-TE header pulse.
 *
 * @param pulses Input pulse array.
 * @param pulse_count Number of pulses.
 * @param start_index Output index of the preamble start.
 *
 * @return STATUS_OK when a valid frame start is found.
 */
static status_t find_frame_start(
    const rf_pulse_t pulses[],
    uint32_t pulse_count,
    uint32_t *start_index)
{
    if (pulses == NULL || start_index == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (pulse_count < HCS301_MAX_PULSES)
    {
        return STATUS_ERROR;
    }

    for (uint32_t start = 0U;
         start <= pulse_count - HCS301_MAX_PULSES;
         start++)
    {
        bool level = true;
        bool valid = true;

        /*
         * Check the 23-pulse preamble.
         */
        for (uint32_t i = 0U;
             i < HCS301_PREAMBLE_TE;
             i++)
        {
            if (!pulse_matches(
                    &pulses[start + i],
                    level,
                    HCS301_TE_US))
            {
                valid = false;
                break;
            }

            level = !level;
        }

        if (!valid)
        {
            continue;
        }

        /*
         * The preamble is followed by
         * a LOW 10-TE header pulse.
         */
        if (!pulse_matches(
                &pulses[start + HCS301_PREAMBLE_TE],
                false,
                HCS301_HEADER_TE * HCS301_TE_US))
        {
            continue;
        }

        *start_index = start;

        return STATUS_OK;
    }

    return STATUS_ERROR;
}

/**
 * @brief Decode one HCS301 data bit.
 *
 * Encoder format:
 *
 * bit 0 = HIGH 1TE + LOW 2TE
 * bit 1 = HIGH 2TE + LOW 1TE
 *
 * @param high_pulse HIGH pulse.
 * @param low_pulse LOW pulse.
 * @param bit Output bit.
 *
 * @return STATUS_OK when valid.
 */
static status_t decode_bit(
    const rf_pulse_t *high_pulse,
    const rf_pulse_t *low_pulse,
    uint8_t *bit)
{
    bool high_is_one_te;
    bool high_is_two_te;
    bool low_is_one_te;
    bool low_is_two_te;

    if (high_pulse == NULL ||
        low_pulse == NULL ||
        bit == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    high_is_one_te =
        duration_matches(
            high_pulse->duration_us,
            HCS301_TE_US);

    high_is_two_te =
        duration_matches(
            high_pulse->duration_us,
            2U * HCS301_TE_US);

    low_is_one_te =
        duration_matches(
            low_pulse->duration_us,
            HCS301_TE_US);

    low_is_two_te =
        duration_matches(
            low_pulse->duration_us,
            2U * HCS301_TE_US);

    if (high_pulse->level != true ||
        low_pulse->level != false)
    {
        return STATUS_ERROR;
    }

    /*
     * Logic 0:
     *
     * HIGH 1TE
     * LOW  2TE
     */
    if (high_is_one_te && low_is_two_te)
    {
        *bit = 0U;
        return STATUS_OK;
    }

    /*
     * Logic 1:
     *
     * HIGH 2TE
     * LOW  1TE
     */
    if (high_is_two_te && low_is_one_te)
    {
        *bit = 1U;
        return STATUS_OK;
    }

    return STATUS_ERROR;
}

/**
 * @brief Decode the 66 HCS301 data bits.
 *
 * @param pulses Input pulse array.
 * @param start_index Preamble start index.
 * @param bits Output bit array.
 *
 * @return STATUS_OK when all bits are valid.
 */
static status_t decode_data(
    const rf_pulse_t pulses[],
    uint32_t start_index,
    uint8_t bits[HCS301_FRAME_BITS])
{
    uint32_t data_start;

    if (pulses == NULL || bits == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    /*
     * Skip:
     *
     * 23 preamble pulses
     * 1 header pulse
     */
    data_start =
        start_index +
        HCS301_PREAMBLE_TE +
        1U;

    for (uint32_t i = 0U;
         i < HCS301_FRAME_BITS;
         i++)
    {
        status_t status;

        status = decode_bit(
            &pulses[data_start + (i * 2U)],
            &pulses[data_start + (i * 2U) + 1U],
            &bits[i]);

        if (status != STATUS_OK)
        {
            return status;
        }
    }

    return STATUS_OK;
}

/**
 * @brief Validate the trailing guard pulse.
 *
 * @param pulses Input pulse array.
 * @param start_index Preamble start index.
 *
 * @return STATUS_OK when guard timing is valid.
 */
static status_t validate_guard(
    const rf_pulse_t pulses[],
    uint32_t start_index)
{
    uint32_t guard_index;

    if (pulses == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    /*
     * Frame layout:
     *
     * 23 preamble
     * 1 header
     * 66 * 2 data pulses
     */
    guard_index =
        start_index +
        HCS301_PREAMBLE_TE +
        1U +
        (HCS301_FRAME_BITS * 2U);

    if (!pulse_matches(
            &pulses[guard_index],
            false,
            HCS301_GUARD_TE * HCS301_TE_US))
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

/**
 * @brief Decode an HCS301 RF pulse train.
 *
 * @param pulses Input RF pulse array.
 * @param pulse_count Number of pulses.
 * @param bits Output 66-bit logical frame.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
status_t hcs301_decoder_decode(
    const rf_pulse_t pulses[],
    uint32_t pulse_count,
    uint8_t bits[HCS301_FRAME_BITS])
{
    uint32_t start_index;
    status_t status;

    if (pulses == NULL || bits == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (pulse_count < HCS301_MAX_PULSES)
    {
        return STATUS_ERROR;
    }

    status = find_frame_start(
        pulses,
        pulse_count,
        &start_index);

    if (status != STATUS_OK)
    {
        return status;
    }

    status = decode_data(
        pulses,
        start_index,
        bits);

    if (status != STATUS_OK)
    {
        return status;
    }

    status = validate_guard(
        pulses,
        start_index);

    if (status != STATUS_OK)
    {
        return status;
    }

    return STATUS_OK;
}