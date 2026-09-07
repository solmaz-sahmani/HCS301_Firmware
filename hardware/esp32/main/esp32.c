#include <stdio.h>

#include "rf_driver.h"
#include "rf_esp32.h"
#include "hcs301_protocol.h"
#include "hcs301_encoder.h"

#define RF_GPIO 4

#define HCS301_TEST_KEY \
    0x0123456789ABCDEFULL

#define HCS301_TEST_SERIAL       0x02AF379U
#define HCS301_TEST_COUNTER      1U
#define HCS301_TEST_DISCRIM      0x379U

void app_main(void)
{
    status_t status;

    printf("\n");
    printf("HCS301 ESP32 Transmitter Test\n");
    printf("==============================\n");

    /*
     * Get shared driver instances.
     */
    rf_driver_t *rf =
        rf_driver_get_instance();

    hcs301_protocol_t *protocol =
        hcs301_protocol_get_instance();

    /*
     * Initialize ESP32 RF backend.
     */
    rf_hal_t rf_hal = {0};

    status = rf_esp32_init(
        &rf_hal,
        RF_GPIO);

    if (status != STATUS_OK)
    {
        printf("RF ESP32 init failed: %d\n", status);
        return;
    }

    /*
     * Initialize RF driver.
     */
    status = rf_driver_init(
        rf,
        &rf_hal);

    if (status != STATUS_OK)
    {
        printf("RF driver init failed: %d\n", status);
        return;
    }

    /*
     * Initialize HCS301 protocol.
     */
    status = hcs301_protocol_init(
        protocol);

    if (status != STATUS_OK)
    {
        printf("Protocol init failed: %d\n", status);
        return;
    }

    /*
     * Create test HCS301 frame.
     */
    hcs301_frame_t frame = {
        .encrypted = 0U,
        .serial = HCS301_TEST_SERIAL,
        .counter = HCS301_TEST_COUNTER,
        .discrimination = HCS301_TEST_DISCRIM,
        .button_status = 0x01U,
        .overflow = 0U,
        .repeat = false,
        .vlow = false
    };

    /*
     * Encode frame into 66 bits.
     */
    uint8_t bits[HCS301_FRAME_BITS] = {0};

    status = hcs301_protocol_encode(
        protocol,
        &frame,
        HCS301_TEST_KEY,
        bits);

    if (status != STATUS_OK)
    {
        printf("Protocol encode failed: %d\n", status);
        return;
    }

    /*
     * Build RF waveform.
     */
    static rf_pulse_t pulses[HCS301_MAX_PULSES];

    uint32_t pulse_count = 0U;

    status = hcs301_encoder_encode(
        bits,
        pulses,
        &pulse_count);

    if (status != STATUS_OK)
    {
        printf("HCS301 encoder failed: %d\n", status);
        return;
    }

    printf("Pulse count: %lu\n",
           (unsigned long)pulse_count);

    /*
     * Transmit HCS301 frame.
     */
    status = rf_driver_transmit(
        rf,
        pulses,
        pulse_count);

    if (status != STATUS_OK)
    {
        printf("RF transmit failed: %d\n", status);
        return;
    }

    printf("HCS301 frame transmitted\n");
}