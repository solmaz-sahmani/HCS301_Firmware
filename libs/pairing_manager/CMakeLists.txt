add_library(pairing_manager
    pairing_manager.c
)

target_include_directories(pairing_manager
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(pairing_manager
    PUBLIC
        err_core
        remote_storage
        led_indicator
        timer_manager
)