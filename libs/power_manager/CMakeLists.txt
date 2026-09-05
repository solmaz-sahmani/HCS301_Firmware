add_library(power_manager
    power_manager.c
)

target_include_directories(power_manager
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(power_manager
    PUBLIC
        err_core
)