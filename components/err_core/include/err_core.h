#ifndef ERR_CORE_H
#define ERR_CORE_H

/**
 * @brief Project status codes.
 */
typedef enum
{
    STATUS_OK = 0,
    STATUS_ERROR,
    STATUS_INVALID_ARG,
    STATUS_TIMEOUT,
    STATUS_NOT_FOUND,
    STATUS_FULL,
    STATUS_ALREADY_EXISTS,
    STATUS_NOT_INITIALIZED,
    STATUS_INVALID_FRAME,
    STATUS_CRC_ERROR

} status_t;

/**
 * @brief Store the last error status.
 *
 * @param status Status to store.
 */
void err_core_set(status_t status);

/**
 * @brief Get the last error status.
 *
 * @return Last stored status.
 */
status_t err_core_get(void);

/**
 * @brief Convert a status code to text.
 *
 * @param status Status code.
 *
 * @return Status text.
 */
const char *err_core_str(status_t status);

#endif