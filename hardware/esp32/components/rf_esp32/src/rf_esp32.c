#include <stddef.h>

#include "driver/rmt_tx.h"
#include "esp_err.h"

#include "rf_esp32.h"

#define RF_ESP32_RESOLUTION_HZ 1000000U
#define RF_ESP32_MEM_SYMBOLS 256U
#define RF_ESP32_QUEUE_DEPTH 1U

typedef struct
{
    rmt_channel_handle_t tx_channel;
    rmt_encoder_handle_t copy_encoder;
} rf_esp32_context_t;

static rf_esp32_context_t rf_esp32_context;

static status_t rf_esp32_transmit(
    void *context,
    const rf_pulse_t *pulses,
    uint32_t count)
{
    if (context == NULL || pulses == NULL || count == 0U)
    {
        return STATUS_INVALID_ARG;
    }

    rf_esp32_context_t *rf =
        (rf_esp32_context_t *)context;

    if (rf->tx_channel == NULL ||
        rf->copy_encoder == NULL)
    {
        return STATUS_NOT_INITIALIZED;
    }

    rmt_symbol_word_t symbols[count];

    for (uint32_t i = 0U; i < count; i++)
    {
        symbols[i].level0 = pulses[i].level;
        symbols[i].duration0 = pulses[i].duration_us;
        symbols[i].level1 = 0U;
        symbols[i].duration1 = 0U;
    }

    rmt_transmit_config_t transmit_config = {
        .loop_count = 0,
    };

    esp_err_t result = rmt_transmit(
        rf->tx_channel,
        rf->copy_encoder,
        symbols,
        count * sizeof(rmt_symbol_word_t),
        &transmit_config);

    if (result != ESP_OK)
    {
        return STATUS_ERROR;
    }

    result = rmt_tx_wait_all_done(
        rf->tx_channel,
        -1);

    if (result != ESP_OK)
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

status_t rf_esp32_init(
    rf_hal_t *hal,
    int gpio_num)
{
    if (hal == NULL || gpio_num < 0)
    {
        return STATUS_INVALID_ARG;
    }

    rmt_tx_channel_config_t channel_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = gpio_num,
        .mem_block_symbols = RF_ESP32_MEM_SYMBOLS,
        .resolution_hz = RF_ESP32_RESOLUTION_HZ,
        .trans_queue_depth = RF_ESP32_QUEUE_DEPTH,
    };

    esp_err_t result = rmt_new_tx_channel(
        &channel_config,
        &rf_esp32_context.tx_channel);

    if (result != ESP_OK)
    {
        return STATUS_ERROR;
    }

    rmt_copy_encoder_config_t encoder_config;

    result = rmt_new_copy_encoder(
        &encoder_config,
        &rf_esp32_context.copy_encoder);

    if (result != ESP_OK)
    {
        return STATUS_ERROR;
    }

    result = rmt_enable(
        rf_esp32_context.tx_channel);

    if (result != ESP_OK)
    {
        return STATUS_ERROR;
    }

    hal->transmit = rf_esp32_transmit;
    hal->context = &rf_esp32_context;

    return STATUS_OK;
}