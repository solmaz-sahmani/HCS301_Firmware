cmake_minimum_required(VERSION 3.22)

# ============================================================
# Project configuration
# ============================================================

set(CMAKE_PROJECT_NAME stm32)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${CMAKE_PROJECT_NAME} C ASM)

message(STATUS "========================================")
message(STATUS "Project   : ${CMAKE_PROJECT_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "ASM compiler: ${CMAKE_ASM_COMPILER}")
message(STATUS "========================================")


# ============================================================
# STM32 target
# ============================================================

add_executable(${CMAKE_PROJECT_NAME}

    # --------------------------------------------------------
    # Application
    # --------------------------------------------------------

    Core/Src/main.c

    # --------------------------------------------------------
    # Hardware-specific drivers
    # --------------------------------------------------------

    Core/Src/rf_stm32.c

    # --------------------------------------------------------
    # Shared drivers
    # --------------------------------------------------------

    ../../drivers/err_core/src/err_core.c
    ../../drivers/rf_driver/src/rf_driver.c
)


# ============================================================
# STM32CubeMX generated code
# ============================================================

add_subdirectory(
    cmake/stm32cubemx
)


# ============================================================
# Include directories
# ============================================================

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE

    # Application
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc

    # Shared drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/err_core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/rf_driver/include
)


# ============================================================
# Compile definitions
# ============================================================

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE

    DEBUG

    STM32F030x6

    USE_HAL_DRIVER
)


# ============================================================
# Compiler options
# ============================================================

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE

    -Wall
    -Wextra

    -ffunction-sections
    -fdata-sections

    -fstack-usage
)


# ============================================================
# Libraries
# ============================================================

target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    m
)


# ============================================================
# Output files
# ============================================================

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES

    SUFFIX ".elf"

)


# ============================================================
# Generate HEX and BIN
# ============================================================

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD

    COMMAND ${CMAKE_OBJCOPY}
        -O ihex
        $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
        ${CMAKE_PROJECT_NAME}.hex

    COMMAND ${CMAKE_OBJCOPY}
        -O binary
        $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
        ${CMAKE_PROJECT_NAME}.bin

    COMMAND ${CMAKE_SIZE}
        $<TARGET_FILE:${CMAKE_PROJECT_NAME}>

    COMMENT "Generating HEX and BIN files..."
)