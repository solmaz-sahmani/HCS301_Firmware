add_library(cli_service
    cli_service.c
)

target_include_directories(cli_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cli_service
    PUBLIC
        err_core
        device_config
)