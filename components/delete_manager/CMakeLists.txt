idf_component_register(
    SRCS
        "src/delete_manager.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        err_core
        remote_storage
)