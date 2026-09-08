/**
 * @file rf_esp32.c
 * @brief ESP32 RMT-based RF pulse transmitter implementation.
 */

#include <stddef.h>

#include "driver/rmt_tx.h"
#include "esp_err.h"

#include "rf_esp32.h"

#define RF_ESP32_RESOLUTION_HZ 1000000U   /**< RMT tick resolution: 1 tick = 1 us */
#define RF_ESP32_MEM_SYMBOLS   256U       /**< RMT hardware memory block size (symbols) */
#define RF_ESP32_QUEUE_DEPTH   1U         /**< RMT TX transaction queue depth */

#define RF_ESP32_MAX_PULSES    157U       /**< Max pulses accepted per transmit() call */
#define RF_ESP32_MAX_SYMBOLS   ((RF_ESP32_MAX_PULSES + 1U) / 2U) /**< Symbols needed to hold RF_ESP32_MAX_PULSES pulses (2 pulses per symbol) */

/**
 * @brief Driver context for the ESP32 RMT RF transmitter.
 */
typedef struct
{
    rmt_channel_handle_t tx_channel;   /**< RMT TX channel handle */
    rmt_encoder_handle_t copy_encoder; /**< RMT copy encoder handle */

    rmt_symbol_word_t symbols[RF_ESP32_MAX_SYMBOLS]; /**< Scratch buffer for pulse-to-symbol conversion */
} rf_esp32_context_t;

static rf_esp32_context_t rf_esp32_context; /**< Singleton driver instance */

/**
 * @brief Convert an RF pulse array into RMT symbols and transmit them.
 *
 * @param context Pointer to an rf_esp32_context_t, previously set up by rf_esp32_init().
 * @param pulses  Array of pulses to transmit.
 * @param count   Number of pulses in @p pulses (must be <= RF_ESP32_MAX_PULSES).
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
static status_t rf_esp32_transmit(
    void *context,
    const rf_pulse_t *pulses,
    uint32_t count)
{
    if (context == NULL ||
        pulses == NULL ||
        count == 0U)
    {
        return STATUS_INVALID_ARG;
    }

    if (count > RF_ESP32_MAX_PULSES)
    {
        return STATUS_FULL;
    }

    rf_esp32_context_t *rf =
        (rf_esp32_context_t *)context;

    if (rf->tx_channel == NULL ||
        rf->copy_encoder == NULL)
    {
        return STATUS_NOT_INITIALIZED;
    }

    /* Two pulses are packed into each RMT symbol (level0/duration0, level1/duration1). */
    uint32_t symbol_count =
        (count + 1U) / 2U;

    for (uint32_t i = 0U; i < symbol_count; i++)
    {
        uint32_t pulse_index = i * 2U;

        rf->symbols[i].level0 =
            pulses[pulse_index].level;

        rf->symbols[i].duration0 =
            pulses[pulse_index].duration_us;

        if ((pulse_index + 1U) < count)
        {
            rf->symbols[i].level1 =
                pulses[pulse_index + 1U].level;

            rf->symbols[i].duration1 =
                pulses[pulse_index + 1U].duration_us;
        }
        else
        {
            /* Odd pulse count: zero duration1 marks end of transmission for RMT. */
            rf->symbols[i].level1 = 0U;
            rf->symbols[i].duration1 = 0U;
        }
    }

    rmt_transmit_config_t transmit_config = {
        .loop_count = 0,
    };

    esp_err_t result = rmt_transmit(
        rf->tx_channel,
        rf->copy_encoder,
        rf->symbols,
        symbol_count * sizeof(rmt_symbol_word_t),
        &transmit_config);

    if (result != ESP_OK)
    {
        return STATUS_ERROR;
    }

    /* Block until transmission completes so the symbol buffer can be safely reused. */
    result = rmt_tx_wait_all_done(
        rf->tx_channel,
        -1);

    if (result != ESP_OK)
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

/**
 * @brief Initialize the ESP32 RMT RF transmitter and bind it to a HAL instance.
 *
 * @param hal      HAL instance to populate with transmit() and context.
 * @param gpio_num GPIO pin used for RF output.
 *
 * @return STATUS_OK on success, or an error status otherwise.
 */
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

    /* Zero-initialize: passing uninitialized config to the driver is undefined behavior. */
    rmt_copy_encoder_config_t encoder_config = {0};

    result = rmt_new_copy_encoder(
        &encoder_config,
        &rf_esp32_context.copy_encoder);

    if (result != ESP_OK)
    {
        /* Roll back the channel already created to avoid a resource leak. */
        rmt_del_channel(rf_esp32_context.tx_channel);
        rf_esp32_context.tx_channel = NULL;
        return STATUS_ERROR;
    }

    result = rmt_enable(
        rf_esp32_context.tx_channel);

    if (result != ESP_OK)
    {
        /* Roll back both encoder and channel on failure. */
        rmt_del_encoder(rf_esp32_context.copy_encoder);
        rf_esp32_context.copy_encoder = NULL;

        rmt_del_channel(rf_esp32_context.tx_channel);
        rf_esp32_context.tx_channel = NULL;
        return STATUS_ERROR;
    }

    hal->transmit = rf_esp32_transmit;
    hal->context = &rf_esp32_context;

    return STATUS_OK;
}