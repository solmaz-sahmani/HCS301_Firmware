#include <stddef.h>

#include "hcs301_encoder.h"

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

static status_t add_te(
    rf_pulse_t pulses[],
    uint32_t *count,
    bool level,
    uint32_t te_count)
{
    uint32_t duration_us =
        te_count * HCS301_TE_US;

    return add_pulse(
        pulses,
        count,
        level,
        (uint16_t)duration_us);
}

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

static status_t add_bit(
    rf_pulse_t pulses[],
    uint32_t *count,
    uint8_t bit)
{
    if (bit == 0U)
    {
        /*
         * Logic 0:
         * 1 TE high
         * 2 TE low
         */
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

    /*
     * Logic 1:
     * 2 TE high
     * 1 TE low
     */
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