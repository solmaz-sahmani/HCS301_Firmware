idf_component_register(
    SRCS
        "src/hcs301_decoder.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        err_core
        rf_driver
        hcs301_encoder
)