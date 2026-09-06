#include "hcs301_rf_tx.h"

#include <stdbool.h>
#include <stddef.h>

#include "driver/gpio.h"
#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"
#include "esp_check.h"

#include "../../common/hcs301_waveform.h"

#define RMT_RESOLUTION_HZ 1000000U

/*
 * Preamble:
 *   23 TE = 11 complete high/low pairs + final high TE.
 *
 * Header:
 *   10 TE high + 1 TE low.
 *
 * Data:
 *   66 symbols, one symbol per bit.
 *
 * Guard:
 *   39 TE low.
 */
#define RMT_SYMBOL_COUNT (12U + 1U + 66U + 1U)

static const char *TAG = "hcs301_rf_tx";

static rmt_channel_handle_t tx_channel;
static rmt_encoder_handle_t copy_encoder;
static rmt_symbol_word_t symbols[RMT_SYMBOL_COUNT];
static bool initialized;

static void set_symbol(
    rmt_symbol_word_t *symbol,
    uint32_t duration0,
    uint32_t level0,
    uint32_t duration1,
    uint32_t level1)
{
    symbol->duration0 = duration0;
    symbol->level0 = level0;
    symbol->duration1 = duration1;
    symbol->level1 = level1;
}

static void build_waveform(const uint8_t bits[66])
{
    size_t index = 0U;

    /* 22 TE alternating preamble. */
    for (uint32_t i = 0U; i < 11U; ++i)
    {
        set_symbol(
            &symbols[index++],
            HCS301_WAVEFORM_TE_US,
            1U,
            HCS301_WAVEFORM_TE_US,
            0U);
    }

    /* Final 1 TE of the 23 TE preamble. */
    set_symbol(
        &symbols[index++],
        HCS301_WAVEFORM_TE_US,
        1U,
        0U,
        0U);

    /* Header. */
    set_symbol(
        &symbols[index++],
        HCS301_WAVEFORM_HEADER_TE * HCS301_WAVEFORM_TE_US,
        1U,
        HCS301_WAVEFORM_TE_US,
        0U);

    /* 66 LSB-first data bits. */
    for (uint32_t i = 0U; i < HCS301_WAVEFORM_BITS; ++i)
    {
        bool bit = bits[i] != 0U;

        set_symbol(
            &symbols[index++],
            hcs301_waveform_high_us(bit),
            1U,
            hcs301_waveform_low_us(bit),
            0U);
    }

    /* Guard time. */
    set_symbol(
        &symbols[index],
        HCS301_WAVEFORM_GUARD_TE * HCS301_WAVEFORM_TE_US,
        0U,
        0U,
        0U);
}

esp_err_t hcs301_rf_tx_init(void)
{
    if (initialized)
    {
        return ESP_OK;
    }

    rmt_tx_channel_config_t channel_config = {
        .gpio_num = HCS301_RF_TX_GPIO,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_RESOLUTION_HZ,
        .mem_block_symbols = 128U,
        .trans_queue_depth = 1U,
        .flags.invert_out = false,
        .flags.with_dma = false
    };

    ESP_RETURN_ON_ERROR(
        rmt_new_tx_channel(&channel_config, &tx_channel),
        TAG,
        "RMT channel creation failed");

    rmt_copy_encoder_config_t encoder_config = {0};

    ESP_RETURN_ON_ERROR(
        rmt_new_copy_encoder(&encoder_config, &copy_encoder),
        TAG,
        "RMT encoder creation failed");

    ESP_RETURN_ON_ERROR(
        rmt_enable(tx_channel),
        TAG,
        "RMT enable failed");

    gpio_set_direction(HCS301_RF_TX_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(HCS301_RF_TX_GPIO, 0);

    initialized = true;
    return ESP_OK;
}

esp_err_t hcs301_rf_tx_send(const uint8_t bits[66])
{
    if (!initialized || bits == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    build_waveform(bits);

    rmt_transmit_config_t transmit_config = {
        .loop_count = 0
    };

    return rmt_transmit(
        tx_channel,
        copy_encoder,
        symbols,
        sizeof(symbols),
        &transmit_config);
}
