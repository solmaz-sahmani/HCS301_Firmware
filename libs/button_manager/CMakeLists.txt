add_library(button_manager
    button_manager.c
)

target_include_directories(button_manager
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(button_manager
    PUBLIC
        err_core
)