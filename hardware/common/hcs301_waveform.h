#ifndef HCS301_WAVEFORM_H
#define HCS301_WAVEFORM_H

#include <stdbool.h>
#include <stdint.h>

#define HCS301_WAVEFORM_BITS          66U
#define HCS301_WAVEFORM_TE_US         400U
#define HCS301_WAVEFORM_PREAMBLE_TE   23U
#define HCS301_WAVEFORM_HEADER_TE     10U
#define HCS301_WAVEFORM_GUARD_TE      39U

static inline uint32_t hcs301_waveform_high_us(bool bit)
{
    return bit ? (2U * HCS301_WAVEFORM_TE_US)
               : HCS301_WAVEFORM_TE_US;
}

static inline uint32_t hcs301_waveform_low_us(bool bit)
{
    return bit ? HCS301_WAVEFORM_TE_US
               : (2U * HCS301_WAVEFORM_TE_US);
}

#endif
