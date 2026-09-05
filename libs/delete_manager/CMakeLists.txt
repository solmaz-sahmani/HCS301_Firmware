add_library(delete_manager
    delete_manager.c
)

target_include_directories(delete_manager
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(delete_manager
    PUBLIC
        err_core
        remote_storage
)