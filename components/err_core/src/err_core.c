#include "err_core.h"

static status_t last_error = STATUS_OK;

/**
 * @brief Store the last error status.
 *
 * @param status Status to store.
 */
void err_core_set(status_t status)
{
    last_error = status;
}

/**
 * @brief Get the last error status.
 *
 * @return Last stored status.
 */
status_t err_core_get(void)
{
    return last_error;
}

/**
 * @brief Convert a status code to text.
 *
 * @param status Status code.
 *
 * @return Status text.
 */
const char *err_core_str(status_t status)
{
    switch (status)
    {
        case STATUS_OK:
            return "OK";

        case STATUS_ERROR:
            return "ERROR";

        case STATUS_INVALID_ARG:
            return "INVALID_ARG";

        case STATUS_TIMEOUT:
            return "TIMEOUT";

        case STATUS_NOT_FOUND:
            return "NOT_FOUND";

        case STATUS_FULL:
            return "FULL";

        case STATUS_ALREADY_EXISTS:
            return "ALREADY_EXISTS";

        case STATUS_NOT_INITIALIZED:
            return "NOT_INITIALIZED";

        case STATUS_INVALID_FRAME:
            return "INVALID_FRAME";

        case STATUS_CRC_ERROR:
            return "CRC_ERROR";

        default:
            return "UNKNOWN";
    }
}