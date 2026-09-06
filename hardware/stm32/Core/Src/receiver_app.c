#include "receiver_app.h"

#include <string.h>

#include "device_config.h"
#include "hcs301_protocol.h"
#include "remote_storage.h"
#include "pairing_manager.h"
#include "delete_manager.h"
#include "led_indicator.h"
#include "rf_driver_stm32.h"

static device_config_t *config;
static hcs301_protocol_t *protocol;
static remote_storage_t *storage;

void receiver_app_init(void)
{
    device_config_init(
        device_config_get_instance());

    config = device_config_get_instance();

    device_config_set_key(
        config,
        0x0123456789ABCDEFULL);

    hcs301_protocol_init(
        hcs301_protocol_get_instance());

    protocol = hcs301_protocol_get_instance();

    remote_storage_init(
        remote_storage_get_instance());

    storage = remote_storage_get_instance();

    led_indicator_init(
        led_indicator_get_instance());

    rf_driver_stm32_init();
}

void receiver_app_process(void)
{
    uint8_t bits[66];

    hcs301_frame_t frame;

    remote_info_t remote;

    if (!rf_driver_stm32_receive(bits))
    {
        return;
    }

    if (hcs301_protocol_decode(
            protocol,
            bits,
            device_config_get_key(config),
            &frame) != STATUS_OK)
    {
        return;
    }

    remote.serial = frame.serial;

    remote.counter = frame.counter;

    if (remote_storage_find(
            storage,
            remote.serial,
            NULL) != STATUS_OK)
    {
        return;
    }

    switch (frame.button)
    {
        case 0:
            led_indicator_set(
                led_indicator_get_instance(),
                0,
                true);
            break;

        case 1:
            led_indicator_set(
                led_indicator_get_instance(),
                1,
                true);
            break;

        case 2:
            led_indicator_set(
                led_indicator_get_instance(),
                2,
                true);
            break;

        case 3:
            led_indicator_set(
                led_indicator_get_instance(),
                3,
                true);
            break;

        default:
            break;
    }
}