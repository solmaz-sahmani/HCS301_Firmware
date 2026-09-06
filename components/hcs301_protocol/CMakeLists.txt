idf_component_register(
    SRCS
        "src/hcs301_protocol.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        err_core
        keeloq_sw
        remote_storage
)