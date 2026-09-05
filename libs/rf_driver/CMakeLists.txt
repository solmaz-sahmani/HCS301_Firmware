add_library(rf_driver
    rf_driver.c
)

target_include_directories(rf_driver
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rf_driver
    PUBLIC
        err_core
)