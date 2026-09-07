idf_component_register(
    SRCS
        "src/hcs301_encoder.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        err_core
        rf_driver
)