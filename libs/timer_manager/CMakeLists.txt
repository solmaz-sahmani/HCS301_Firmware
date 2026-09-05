add_library(timer_manager
    timer_manager.c
)

target_include_directories(timer_manager
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(timer_manager
    PUBLIC
        err_core
)