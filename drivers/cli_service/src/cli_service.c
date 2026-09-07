#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>

#include "cli_service.h"
#include "device_config.h"

#define CLI_BUFFER_SIZE 64U

struct cli_service
{
    bool initialized;
    bool running;
};

static cli_service_t cli_service_instance;

static bool parse_hex_u32(
    const char *text,
    uint32_t *value)
{
    unsigned int parsed;

    if (text == NULL || value == NULL)
    {
        return false;
    }

    if (sscanf(text, "%x", &parsed) != 1)
    {
        return false;
    }

    *value = (uint32_t)parsed;

    return true;
}

static bool parse_hex_u64(
    const char *text,
    uint64_t *value)
{
    unsigned long long parsed;

    if (text == NULL || value == NULL)
    {
        return false;
    }

    if (sscanf(text, "%llx", &parsed) != 1)
    {
        return false;
    }

    *value = (uint64_t)parsed;

    return true;
}

static void print_help(void)
{
    printf("\n");
    printf("Commands:\n");
    printf("  SET KEY <64-bit-hex>\n");
    printf("  GET KEY\n");
    printf("  SET SERIAL <28-bit-hex>\n");
    printf("  GET SERIAL\n");
    printf("  SET DISC <10-bit-hex>\n");
    printf("  GET DISC\n");
    printf("  HELP\n");
    printf("  EXIT\n");
    printf("\n");
}

static void process_command(
    cli_service_t *service,
    char *command)
{
    char value[32];
    device_config_t *config;

    config = device_config_get_instance();

    if (strcmp(command, "GET KEY") == 0)
    {
        uint64_t key;

        device_config_get_key(
            config,
            &key);

        printf(
            "KEY: 0x%016llX\n",
            (unsigned long long)key);

        return;
    }

    if (strcmp(command, "GET SERIAL") == 0)
    {
        uint32_t serial;

        device_config_get_serial(
            config,
            &serial);

        printf(
            "SERIAL: 0x%08" PRIX32 "\n",
            serial
        );

        return;
    }

    if (strcmp(command, "GET DISC") == 0)
    {
        uint16_t discrimination;

        device_config_get_discrimination(
            config,
            &discrimination);

        printf(
            "DISC: 0x%03X\n",
            discrimination);

        return;
    }

    if (sscanf(
            command,
            "SET KEY %31s",
            value) == 1)
    {
        uint64_t key;

        if (!parse_hex_u64(value, &key))
        {
            printf("ERROR\n");
            return;
        }

        if (device_config_set_key(
                config,
                key) != STATUS_OK)
        {
            printf("ERROR\n");
            return;
        }

        printf("OK\n");

        return;
    }

    if (sscanf(
            command,
            "SET SERIAL %31s",
            value) == 1)
    {
        uint32_t serial;

        if (!parse_hex_u32(value, &serial))
        {
            printf("ERROR\n");
            return;
        }

        if (device_config_set_serial(
                config,
                serial) != STATUS_OK)
        {
            printf("ERROR\n");
            return;
        }

        printf("OK\n");

        return;
    }

    if (sscanf(
            command,
            "SET DISC %31s",
            value) == 1)
    {
        uint32_t discrimination;

        if (!parse_hex_u32(
                value,
                &discrimination))
        {
            printf("ERROR\n");
            return;
        }

        if (device_config_set_discrimination(
                config,
                (uint16_t)discrimination) != STATUS_OK)
        {
            printf("ERROR\n");
            return;
        }

        printf("OK\n");

        return;
    }

    if (strcmp(command, "HELP") == 0)
    {
        print_help();
        return;
    }

    if (strcmp(command, "EXIT") == 0)
    {
        service->running = false;

        printf("OK\n");

        return;
    }

    printf("ERROR\n");
}

cli_service_t *cli_service_get_instance(void)
{
    return &cli_service_instance;
}

status_t cli_service_init(
    cli_service_t *service)
{
    if (service == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    service->initialized = true;
    service->running = true;

    return STATUS_OK;
}

status_t cli_service_process(
    cli_service_t *service)
{
    char buffer[CLI_BUFFER_SIZE];
    size_t length;

    if (service == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (!service->initialized)
    {
        return STATUS_NOT_INITIALIZED;
    }

    if (!service->running)
    {
        return STATUS_OK;
    }

    printf("> ");
    fflush(stdout);

    if (fgets(
            buffer,
            sizeof(buffer),
            stdin) == NULL)
    {
        return STATUS_ERROR;
    }

    length = strlen(buffer);

    if (length > 0U &&
        buffer[length - 1U] == '\n')
    {
        buffer[length - 1U] = '\0';
    }

    if (length > 1U &&
        buffer[length - 2U] == '\r')
    {
        buffer[length - 2U] = '\0';
    }

    process_command(
        service,
        buffer);

    return STATUS_OK;
}

status_t cli_service_is_running(
    cli_service_t *service,
    bool *running)
{
    if (service == NULL || running == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    *running = service->running;

    return STATUS_OK;
}