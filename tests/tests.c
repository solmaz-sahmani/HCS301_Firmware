#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "device_config.h"
#include "err_core.h"
#include "hcs301_protocol.h"
#include "keeloq_sw.h"
#include "remote_storage.h"
#include "rf_driver.h"

static int tests_passed;
static int tests_failed;

static void check(
    const char *name,
    bool condition)
{
    if (condition)
    {
        printf("[PASS] %s\n", name);
        tests_passed++;
    }
    else
    {
        printf("[FAIL] %s\n", name);
        tests_failed++;
    }
}

static void test_device_config(void)
{
    device_config_t *config;

    uint64_t key;
    uint32_t serial;
    uint16_t discrimination;

    config =
        device_config_get_instance();

    printf("\n");
    printf("DEVICE CONFIG\n");
    printf("-------------\n");

    check(
        "Config init",
        device_config_init(config) == STATUS_OK);

    check(
        "Set key",
        device_config_set_key(
            config,
            0x0123456789ABCDEFULL) == STATUS_OK);

    check(
        "Get key",
        device_config_get_key(
            config,
            &key) == STATUS_OK);

    check(
        "Key value",
        key == 0x0123456789ABCDEFULL);

    check(
        "Set serial",
        device_config_set_serial(
            config,
            0x01234567U) == STATUS_OK);

    check(
        "Get serial",
        device_config_get_serial(
            config,
            &serial) == STATUS_OK);

    check(
        "Serial value",
        serial == 0x01234567U);

    check(
        "Set discrimination",
        device_config_set_discrimination(
            config,
            0x167U) == STATUS_OK);

    check(
        "Get discrimination",
        device_config_get_discrimination(
            config,
            &discrimination) == STATUS_OK);

    check(
        "Discrimination value",
        discrimination == 0x167U);
}

static void test_keeloq(void)
{
    uint32_t encrypted;
    uint32_t decrypted;

    printf("\n");
    printf("KEELOQ\n");
    printf("------\n");

    check(
        "Encrypt",
        keeloq_encrypt(
            0x12345678U,
            0x0123456789ABCDEFULL,
            &encrypted) == STATUS_OK);

    check(
        "Known ciphertext",
        encrypted == 0xB909A03BU);

    check(
        "Decrypt",
        keeloq_decrypt(
            encrypted,
            0x0123456789ABCDEFULL,
            &decrypted) == STATUS_OK);

    check(
        "Plaintext restored",
        decrypted == 0x12345678U);
}

static void test_hcs301_protocol(void)
{
    hcs301_protocol_t *protocol;

    hcs301_frame_t tx;
    hcs301_frame_t rx;

    remote_info_t remote;

    uint8_t bits[HCS301_FRAME_BITS];

    const uint64_t key =
        0x0123456789ABCDEFULL;

    status_t status;

    protocol =
        hcs301_protocol_get_instance();

    printf("\n");
    printf("HCS301 PROTOCOL\n");
    printf("---------------\n");

    tx.encrypted = 0U;
    tx.serial = 0x01234567U;
    tx.counter = 0x0042U;
    tx.discrimination = 0x167U;
    tx.button_status = 0x01U;
    tx.overflow = 0U;
    tx.repeat = false;
    tx.vlow = false;

    status =
        hcs301_protocol_encode(
            protocol,
            &tx,
            key,
            bits);

    check(
        "Encode frame",
        status == STATUS_OK);

    status =
        hcs301_protocol_decode(
            protocol,
            bits,
            key,
            &rx);

    check(
        "Decode frame",
        status == STATUS_OK);

    check(
        "Serial decoded",
        rx.serial == tx.serial);

    check(
        "Counter decoded",
        rx.counter == tx.counter);

    check(
        "Button decoded",
        rx.button_status ==
            tx.button_status);

    check(
        "Discrimination decoded",
        rx.discrimination ==
            tx.discrimination);

    status =
        hcs301_protocol_verify(
            protocol,
            &rx,
            tx.serial,
            tx.discrimination);

    check(
        "Frame verification",
        status == STATUS_OK);

    status =
        hcs301_protocol_get_remote_info(
            protocol,
            &rx,
            &remote);

    check(
        "Remote conversion",
        status == STATUS_OK);

    check(
        "Remote serial",
        remote.serial == tx.serial);

    check(
        "Remote counter",
        remote.counter == tx.counter);
}

static void test_rf_driver(void)
{
    rf_driver_t *rf;

    rf_pulse_t tx[4];
    rf_pulse_t rx[4];

    uint32_t received;

    rf = rf_driver_get_instance();

    printf("\n");
    printf("RF DRIVER\n");
    printf("---------\n");

    tx[0].duration_us = 100U;
    tx[0].level = 1U;

    tx[1].duration_us = 200U;
    tx[1].level = 0U;

    tx[2].duration_us = 300U;
    tx[2].level = 1U;

    tx[3].duration_us = 400U;
    tx[3].level = 0U;

    check(
        "RF transmit",
        rf_driver_transmit(
            rf,
            tx,
            4U) == STATUS_OK);

    check(
        "RF receive",
        rf_driver_receive(
            rf,
            rx,
            4U,
            &received) == STATUS_OK);

    check(
        "Pulse count",
        received == 4U);

    check(
        "Pulse 0",
        rx[0].duration_us == 100U &&
        rx[0].level == 1U);

    check(
        "Pulse 1",
        rx[1].duration_us == 200U &&
        rx[1].level == 0U);

    check(
        "Pulse 2",
        rx[2].duration_us == 300U &&
        rx[2].level == 1U);

    check(
        "Pulse 3",
        rx[3].duration_us == 400U &&
        rx[3].level == 0U);
}

static void test_storage(void)
{
    remote_storage_t *storage;

    remote_info_t remote;

    storage =
        remote_storage_get_instance();

    printf("\n");
    printf("REMOTE STORAGE\n");
    printf("--------------\n");

    check(
        "Storage init",
        remote_storage_init(storage) ==
            STATUS_OK);

    remote.serial = 0x01234567U;
    remote.counter = 100U;

    check(
        "Add remote",
        remote_storage_add(
            storage,
            remote.serial,
            remote.counter) == STATUS_OK);

    check(
        "Remote count",
        remote_storage_count(storage) == 1U);

    check(
        "Find remote",
        remote_storage_find(
            storage,
            remote.serial,
            &remote) == STATUS_OK);

    check(
        "Remove remote",
        remote_storage_remove(
            storage,
            remote.serial) == STATUS_OK);

    check(
        "Storage empty",
        remote_storage_count(storage) == 0U);
}

static void test_full_flow(void)
{
    hcs301_protocol_t *protocol;
    remote_storage_t *storage;

    hcs301_frame_t frame;
    remote_info_t remote;

    uint8_t bits[HCS301_FRAME_BITS];

    const uint64_t key =
        0x0123456789ABCDEFULL;

    status_t status;

    protocol =
        hcs301_protocol_get_instance();

    storage =
        remote_storage_get_instance();

    printf("\n");
    printf("FULL FLOW\n");
    printf("---------\n");

    frame.serial = 0x00ABCDEFU;
    frame.counter = 10U;
    frame.discrimination = 0x155U;
    frame.button_status = 0x04U;
    frame.overflow = 0U;
    frame.repeat = false;
    frame.vlow = false;
    frame.encrypted = 0U;

    status =
        hcs301_protocol_encode(
            protocol,
            &frame,
            key,
            bits);

    check(
        "Encode remote frame",
        status == STATUS_OK);

    if (status != STATUS_OK)
    {
        return;
    }

    status =
        hcs301_protocol_decode(
            protocol,
            bits,
            key,
            &frame);

    check(
        "Decode remote frame",
        status == STATUS_OK);

    if (status != STATUS_OK)
    {
        return;
    }

    status =
        hcs301_protocol_verify(
            protocol,
            &frame,
            0x00ABCDEFU,
            0x155U);

    check(
        "Verify remote frame",
        status == STATUS_OK);

    if (status != STATUS_OK)
    {
        return;
    }

    status =
        hcs301_protocol_get_remote_info(
            protocol,
            &frame,
            &remote);

    check(
        "Build remote info",
        status == STATUS_OK);

    if (status != STATUS_OK)
    {
        return;
    }

    status =
        remote_storage_add(
            storage,
            remote.serial,
            remote.counter);

    check(
        "Store decoded remote",
        status == STATUS_OK);

    check(
        "Stored remote exists",
        remote_storage_count(storage) == 1U);

    status =
        remote_storage_remove(
            storage,
            remote.serial);

    check(
        "Delete stored remote",
        status == STATUS_OK);

    check(
        "Remote removed",
        remote_storage_count(storage) == 0U);
}

int main(void)
{
    status_t status;

    printf("HCS301 Integration Test\n");
    printf("=======================\n");

    status =
        device_config_init(
            device_config_get_instance());

    check(
        "Device config init",
        status == STATUS_OK);

    status =
        hcs301_protocol_init(
            hcs301_protocol_get_instance());

    check(
        "HCS301 protocol init",
        status == STATUS_OK);

    status =
        rf_driver_init(
            rf_driver_get_instance());

    check(
        "RF driver init",
        status == STATUS_OK);

    status =
        remote_storage_init(
            remote_storage_get_instance());

    check(
        "Remote storage init",
        status == STATUS_OK);

    test_device_config();
    test_keeloq();
    test_hcs301_protocol();
    test_rf_driver();
    test_storage();
    test_full_flow();

    printf("\n");
    printf("=======================\n");

    printf(
        "Passed: %d\n",
        tests_passed);

    printf(
        "Failed: %d\n",
        tests_failed);

    if (tests_failed == 0)
    {
        printf("RESULT: PASS\n");

        return 0;
    }

    printf("RESULT: FAIL\n");

    return 1;
}