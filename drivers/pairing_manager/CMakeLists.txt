idf_component_register(
    SRCS
        "src/pairing_manager.c"

    INCLUDE_DIRS
        "include"

    PRIV_REQUIRES
        remote_storage
        timer_manager
        led_indicator
)