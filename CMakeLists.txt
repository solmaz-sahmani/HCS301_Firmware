cmake_minimum_required(VERSION 3.20)

project(hcs301_firmware C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(libs/err_core)
add_subdirectory(libs/remote_storage)
add_subdirectory(libs/button_manager)
add_subdirectory(libs/led_indicator)
add_subdirectory(libs/rf_driver)
add_subdirectory(libs/keeloq_sw)
add_subdirectory(libs/pairing_manager)
add_subdirectory(libs/timer_manager)
add_subdirectory(libs/delete_manager)
add_subdirectory(libs/cli_service)
add_subdirectory(libs/power_manager)
add_subdirectory(libs/device_config)
add_subdirectory(libs/hcs301_protocol)

add_subdirectory(app)
add_subdirectory(tests)