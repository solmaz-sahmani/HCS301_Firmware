#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "button_manager.h"
#include "cli_service.h"
#include "delete_manager.h"
#include "device_config.h"
#include "err_core.h"
#include "hcs301_protocol.h"
#include "keeloq_sw.h"
#include "led_indicator.h"
#include "pairing_manager.h"
#include "power_manager.h"
#include "remote_storage.h"
#include "rf_driver.h"
#include "timer_manager.h"

static status_t mock_power_sleep(void *context)
{
    (void)context;

    printf("[POWER] Sleep\n");

    return STATUS_OK;
}

static status_t mock_power_wake(void *context)
{
    (void)context;

    printf("[POWER] Wake\n");

    return STATUS_OK;
}

static status_t system_init(void)
{
    status_t status;

    power_hal_t power_hal;

    power_hal.sleep = mock_power_sleep;
    power_hal.wake = mock_power_wake;
    power_hal.context = NULL;

    status =
        remote_storage_init(
            remote_storage_get_instance());

    if (status != STATUS_OK)
    {
        return status;
    }

    status =
        button_manager_init(
            button_manager_get_instance());

    if (status != STATUS_OK)
    {
        return status;
    }

    status =
        led_indicator_init(
            led_indicator_get_instance());

    if (status != STATUS_OK)
    {
        return status;
    }

    status =
        timer_manager_init(
            timer_manager_get_instance());

    if (status != STATUS_OK)
    {
        return status;
    }

    status =
        rf_driver_init(
            rf_driver_get_instance());

    if (status != STATUS_OK)
    {
        return status;
    }

    status =
        pairing_manager_init(
            pairing_manager_get_instance());

    if (status != STATUS_OK)
    {
        return status;
    }

    status =
        delete_manager_init(
            delete_manager_get_instance());

    if (status != STATUS_OK)
    {
        return status;
    }

    status =
        device_config_init(
            device_config_get_instance());

    if (status != STATUS_OK)
    {
        return status;
    }

    status =
        hcs301_protocol_init(
            hcs301_protocol_get_instance());

    if (status != STATUS_OK)
    {
        return status;
    }

    status =
        cli_service_init(
            cli_service_get_instance());

    if (status != STATUS_OK)
    {
        return status;
    }

    status =
        power_manager_init(
            power_manager_get_instance(),
            &power_hal);

    return status;
}

static void application_process(void)
{
    pairing_manager_process(
        pairing_manager_get_instance());
}

static void application_run(void)
{
    bool running;

    while (true)
    {
        application_process();

        cli_service_is_running(
            cli_service_get_instance(),
            &running);

        if (!running)
        {
            break;
        }

        if (cli_service_process(
                cli_service_get_instance()) != STATUS_OK)
        {
            break;
        }
    }
}

int main(void)
{
    status_t status;

    printf("HCS301 Firmware MVP\n");
    printf("===================\n");

    status = system_init();

    printf(
        "System Init : %s\n",
        err_core_str(status));

    if (status != STATUS_OK)
    {
        return 1;
    }

    application_run();

    printf("\nApplication stopped.\n");

    return 0;
}