#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include <stdint.h>

#include "err_core.h"

typedef struct device_config device_config_t;

/**
 * @brief Get the device configuration instance.
 *
 * @return Device configuration instance.
 */
device_config_t *device_config_get_instance(void);

/**
 * @brief Initialize device configuration.
 *
 * @param config Device configuration instance.
 *
 * @return Operation status.
 */
status_t device_config_init(
    device_config_t *config);

/**
 * @brief Set the encryption key.
 *
 * @param config Device configuration instance.
 * @param key 64-bit encryption key.
 *
 * @return Operation status.
 */
status_t device_config_set_key(
    device_config_t *config,
    uint64_t key);

/**
 * @brief Get the encryption key.
 *
 * @param config Device configuration instance.
 * @param key Output encryption key.
 *
 * @return Operation status.
 */
status_t device_config_get_key(
    device_config_t *config,
    uint64_t *key);

/**
 * @brief Set the device serial number.
 *
 * @param config Device configuration instance.
 * @param serial Device serial number.
 *
 * @return Operation status.
 */
status_t device_config_set_serial(
    device_config_t *config,
    uint32_t serial);

/**
 * @brief Get the device serial number.
 *
 * @param config Device configuration instance.
 * @param serial Output serial number.
 *
 * @return Operation status.
 */
status_t device_config_get_serial(
    device_config_t *config,
    uint32_t *serial);

/**
 * @brief Set the discrimination value.
 *
 * @param config Device configuration instance.
 * @param discrimination 10-bit discrimination value.
 *
 * @return Operation status.
 */
status_t device_config_set_discrimination(
    device_config_t *config,
    uint16_t discrimination);

/**
 * @brief Get the discrimination value.
 *
 * @param config Device configuration instance.
 * @param discrimination Output discrimination value.
 *
 * @return Operation status.
 */
status_t device_config_get_discrimination(
    device_config_t *config,
    uint16_t *discrimination);

#endif