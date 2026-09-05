add_library(err_core
    err_core.c
)

target_include_directories(err_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)