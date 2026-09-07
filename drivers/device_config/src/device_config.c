#include <stddef.h>

#include "device_config.h"

struct device_config
{
    uint64_t key;
    uint32_t serial;
    uint16_t discrimination;
};

static device_config_t device_config_instance;

device_config_t *device_config_get_instance(void)
{
    return &device_config_instance;
}

status_t device_config_init(
    device_config_t *config)
{
    if (config == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    config->key = 0U;
    config->serial = 0U;
    config->discrimination = 0U;

    return STATUS_OK;
}

status_t device_config_set_key(
    device_config_t *config,
    uint64_t key)
{
    if (config == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    config->key = key;

    return STATUS_OK;
}

status_t device_config_get_key(
    device_config_t *config,
    uint64_t *key)
{
    if (config == NULL || key == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    *key = config->key;

    return STATUS_OK;
}

status_t device_config_set_serial(
    device_config_t *config,
    uint32_t serial)
{
    if (config == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if ((serial & 0xF0000000U) != 0U)
    {
        return STATUS_INVALID_ARG;
    }

    config->serial = serial;

    return STATUS_OK;
}

status_t device_config_get_serial(
    device_config_t *config,
    uint32_t *serial)
{
    if (config == NULL || serial == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    *serial = config->serial;

    return STATUS_OK;
}

status_t device_config_set_discrimination(
    device_config_t *config,
    uint16_t discrimination)
{
    if (config == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (discrimination > 0x03FFU)
    {
        return STATUS_INVALID_ARG;
    }

    config->discrimination = discrimination;

    return STATUS_OK;
}

status_t device_config_get_discrimination(
    device_config_t *config,
    uint16_t *discrimination)
{
    if (config == NULL || discrimination == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    *discrimination = config->discrimination;

    return STATUS_OK;
}