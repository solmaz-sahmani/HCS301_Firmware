add_executable(hcs301_integration_test
    tests.c
)

target_link_libraries(hcs301_integration_test
    PRIVATE
        err_core
        remote_storage
        rf_driver
        keeloq_sw
        device_config
        hcs301_protocol
)