idf_component_register(
    SRCS "esp32.c"
    INCLUDE_DIRS "."
    REQUIRES
        "rf_esp32"
        "rf_driver"
        "hcs301_protocol"
        "hcs301_encoder"
)