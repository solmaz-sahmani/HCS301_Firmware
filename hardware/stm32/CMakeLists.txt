cmake_minimum_required(VERSION 3.22)

# ============================================================
# Compiler settings
# ============================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME stm32)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(${CMAKE_PROJECT_NAME})

message("Build type: ${CMAKE_BUILD_TYPE}")

enable_language(C ASM)

add_executable(${CMAKE_PROJECT_NAME})


# ============================================================
# STM32CubeMX
# ============================================================

add_subdirectory(cmake/stm32cubemx)


# ============================================================
# Project sources
# ============================================================

target_sources(${CMAKE_PROJECT_NAME} PRIVATE

    # Application
    Core/Src/main.c

    # STM32 RF hardware driver
    Core/Src/rf_stm32.c

    # Shared error driver
    ../../drivers/err_core/src/err_core.c

    # Shared RF driver
    ../../drivers/rf_driver/src/rf_driver.c
)


# ============================================================
# Include directories
# ============================================================

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE

    # Application / hardware
    Core/Inc

    # Shared drivers
    ../../drivers/err_core/include
    ../../drivers/rf_driver/include
)


# ============================================================
# Link directories
# ============================================================

target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
)


# ============================================================
# Compile definitions
# ============================================================

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
)


# ============================================================
# Remove wrong library dependency
# ============================================================

list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)


# ============================================================
# Libraries
# ============================================================

target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
)