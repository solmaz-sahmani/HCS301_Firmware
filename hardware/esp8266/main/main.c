#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "device_config.h"
#include "hcs301_protocol.h"
#include "rf_driver_esp32.h"

static const char *TAG = "HCS301_TX";

static device_config_t *config;
static hcs301_protocol_t *protocol;

static void transmitter_init(void)
{
    device_config_init(device_config_get_instance());

    config = device_config_get_instance();

    device_config_set_key(
        config,
        0x0123456789ABCDEFULL);

    device_config_set_serial(
        config,
        0x12345678UL);

    device_config_set_discrimination(
        config,
        0x3FFU);

    hcs301_protocol_init(
        hcs301_protocol_get_instance());

    protocol = hcs301_protocol_get_instance();

    rf_driver_esp32_init();
}

void app_main(void)
{
    hcs301_frame_t frame;

    uint8_t rf_bits[66];

    transmitter_init();

    while (1)
    {
        frame.serial = device_config_get_serial(config);

        frame.counter++;

        frame.button = 0;

        frame.discrimination =
            device_config_get_discrimination(config);

        hcs301_protocol_encode(
            protocol,
            &frame,
            device_config_get_key(config),
            rf_bits);

        ESP_LOGI(TAG,
                 "Send Serial=0x%08X Counter=%lu",
                 frame.serial,
                 frame.counter);

        rf_driver_esp32_transmit_frame(rf_bits);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}