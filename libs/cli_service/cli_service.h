#ifndef CLI_SERVICE_H
#define CLI_SERVICE_H

#include <stdbool.h>

#include "err_core.h"

typedef struct cli_service cli_service_t;

/**
 * @brief Get the CLI service instance.
 *
 * @return CLI service instance.
 */
cli_service_t *cli_service_get_instance(void);

/**
 * @brief Initialize CLI service.
 *
 * @param service CLI service instance.
 *
 * @return Operation status.
 */
status_t cli_service_init(
    cli_service_t *service);

/**
 * @brief Process one CLI command.
 *
 * @param service CLI service instance.
 *
 * @return Operation status.
 */
status_t cli_service_process(
    cli_service_t *service);

/**
 * @brief Check whether the CLI is running.
 *
 * @param service CLI service instance.
 * @param running Output running state.
 *
 * @return Operation status.
 */
status_t cli_service_is_running(
    cli_service_t *service,
    bool *running);

#endif