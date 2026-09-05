add_library(device_config
    device_config.c
)

target_include_directories(device_config
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(device_config
    PUBLIC
        err_core
)