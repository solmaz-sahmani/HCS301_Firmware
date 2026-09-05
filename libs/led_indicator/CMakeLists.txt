add_library(led_indicator
    led_indicator.c
)

target_include_directories(led_indicator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(led_indicator
    PUBLIC
        err_core
)