#ifndef KEELOQ_SW_H
#define KEELOQ_SW_H

#include <stdint.h>

#include "err_core.h"

/**
 * @brief Encrypt a 32-bit value using KeeLoq.
 *
 * @param data Input data.
 * @param key 64-bit encryption key.
 * @param encrypted Output encrypted data.
 *
 * @return Operation status.
 */
status_t keeloq_encrypt(
    uint32_t data,
    uint64_t key,
    uint32_t *encrypted);

/**
 * @brief Decrypt a 32-bit value using KeeLoq.
 *
 * @param data Input encrypted data.
 * @param key 64-bit encryption key.
 * @param decrypted Output decrypted data.
 *
 * @return Operation status.
 */
status_t keeloq_decrypt(
    uint32_t data,
    uint64_t key,
    uint32_t *decrypted);

#endif