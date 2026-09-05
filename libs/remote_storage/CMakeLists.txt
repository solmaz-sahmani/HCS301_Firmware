add_library(remote_storage
    remote_storage.c
)

target_include_directories(remote_storage
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(remote_storage
    PUBLIC
        err_core
)