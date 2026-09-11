idf_component_register(
    SRCS "esp32.c"
    INCLUDE_DIRS "."
    REQUIRES

        "button_input"
        "button_manager"
        "rf_esp32"
        "rf_driver"
        "hcs301_protocol"
        "hcs301_encoder"
        "hcs301_send"

    PRIV_REQUIRES
        esp_driver_gpio
)
