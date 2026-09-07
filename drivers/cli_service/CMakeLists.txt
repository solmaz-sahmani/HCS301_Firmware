idf_component_register(
    SRCS
        "src/cli_service.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        err_core
        device_config
)