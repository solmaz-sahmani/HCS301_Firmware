# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/../../drivers/button_manager"
    "${CMAKE_CURRENT_LIST_DIR}/../../drivers/device_config"
    "${CMAKE_CURRENT_LIST_DIR}/../../drivers/cli_service"
    "${CMAKE_CURRENT_LIST_DIR}/../../drivers/timer_manager"
    "${CMAKE_CURRENT_LIST_DIR}/../../drivers/rf_driver"
    "${CMAKE_CURRENT_LIST_DIR}/../../drivers/err_core"
    "${CMAKE_CURRENT_LIST_DIR}/../../drivers/hcs301_protocol"
    "${CMAKE_CURRENT_LIST_DIR}/../../drivers/keeloq_sw"
    "${CMAKE_CURRENT_LIST_DIR}/../../drivers/hcs301_encoder"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32)
