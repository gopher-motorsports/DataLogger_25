cmake_minimum_required(VERSION 3.22)

# -----------------------------------------------------------------------------
# Language and project configuration
# -----------------------------------------------------------------------------

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_PROJECT_NAME DataLogger25)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(${CMAKE_PROJECT_NAME}
    LANGUAGES C ASM
)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# -----------------------------------------------------------------------------
# Main executable
# -----------------------------------------------------------------------------

add_executable(${CMAKE_PROJECT_NAME})

# This provides:
#   - STM32 HAL
#   - CMSIS/system_stm32f4xx.c
#   - FreeRTOS
#   - FatFs core files
#   - CubeMX-generated application sources
add_subdirectory(cmake/stm32cubemx)

# -----------------------------------------------------------------------------
# Custom project sources
# -----------------------------------------------------------------------------

file(GLOB MAIN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Core/Src/*.c"
    "${CMAKE_SOURCE_DIR}/Core/Src/adbms/*.c"
)

# system_stm32f4xx.c is already compiled by the STM32_Drivers target.
list(REMOVE_ITEM MAIN_SOURCES
    "${CMAKE_SOURCE_DIR}/Core/Src/system_stm32f4xx.c"
)

file(GLOB GOPHERCAN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/../gophercan-lib/*.c"
)

file(GLOB GOPHERSENSE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/../Gopher_Sense/lib/*.c"
    "${CMAKE_SOURCE_DIR}/../Gopher_Sense/Build/*.c"
)

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${MAIN_SOURCES}
        ${GOPHERCAN_SOURCES}
        ${GOPHERSENSE_SOURCES}
)

# Do not manually add:
#
# Middlewares/Third_Party/FatFs/src/option/ccsbcs.c
# Middlewares/Third_Party/FatFs/src/option/syscall.c
#
# They are already compiled by the generated FatFs target.

# -----------------------------------------------------------------------------
# Include directories
# -----------------------------------------------------------------------------

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/../gophercan-lib"

        "${CMAKE_SOURCE_DIR}/../Gopher_Sense"
        "${CMAKE_SOURCE_DIR}/../Gopher_Sense/lib"
        "${CMAKE_SOURCE_DIR}/../Gopher_Sense/Build"
)

# -----------------------------------------------------------------------------
# Libraries
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
# Compiler definitions
# -----------------------------------------------------------------------------

target_compile_definitions(${CMAKE_PROJECT_NAME}
    PRIVATE
        # Add application-specific definitions here
)

# -----------------------------------------------------------------------------
# Linker options
# -----------------------------------------------------------------------------

target_link_options(${CMAKE_PROJECT_NAME}
    PRIVATE
        -u
        _printf_float
)

# -----------------------------------------------------------------------------
# Generate HEX and BIN output files
# -----------------------------------------------------------------------------

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD

    COMMAND ${CMAKE_OBJCOPY}
        -O ihex
        "$<TARGET_FILE:${CMAKE_PROJECT_NAME}>"
        "${CMAKE_PROJECT_NAME}.hex"

    COMMAND ${CMAKE_OBJCOPY}
        -O binary
        "$<TARGET_FILE:${CMAKE_PROJECT_NAME}>"
        "${CMAKE_PROJECT_NAME}.bin"

    COMMENT "Generating ${CMAKE_PROJECT_NAME}.hex and ${CMAKE_PROJECT_NAME}.bin"
    VERBATIM
)