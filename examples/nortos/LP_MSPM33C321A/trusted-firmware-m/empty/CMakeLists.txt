#-------------------------------------------------------------------------------
# Copyright (c) 2026, Texas Instruments Incorporated
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# This CMakeLists.txt builds the TF-M Non-Secure application for LP_MSPM33C321A.
# It follows the standard TF-M NS application pattern used by tf-m-tests.
#
# Required variables (set via -D on cmake command line):
#   CONFIG_SPE_PATH      Path to SPE api_ns install tree (e.g., build/spe/api_ns)
#   TI_SDK_PATH          Path to MSP-SDK root (e.g., /home/user/ti/msp-sdk)
#   TFM_PLATFORM         Platform name (ti/LP_MSPM33C321A)
#   TFM_SYSTEM_PROCESSOR Processor type (cortex-m33)
#   TFM_SYSTEM_ARCHITECTURE  Architecture (armv8-m.main)
#   CROSS_COMPILE        Toolchain prefix (e.g., /path/to/arm-none-eabi)
#   TFM_TOOLCHAIN_FILE   Path to NS toolchain file (from api_ns/cmake/)
#
# Build:
#   cmake -S ${SDK_ROOT}/examples/trusted-firmware-m/empty/LP_MSPM33C321A -B build/ns \
#         -DCONFIG_SPE_PATH=<SPE_BUILD>/api_ns \
#         -DTFM_TOOLCHAIN_FILE=<SPE_BUILD>/api_ns/cmake/toolchain_ns_GNUARM.cmake \
#         -DCROSS_COMPILE=<path>/arm-none-eabi \
#         -DTI_SDK_PATH=<SDK_ROOT> \
#         -DTFM_PLATFORM=ti/LP_MSPM33C321A \
#         -DTFM_SYSTEM_PROCESSOR=cortex-m33 \
#         -DTFM_SYSTEM_ARCHITECTURE=armv8-m.main \
#         -DTFM_SYSTEM_DSP=OFF \
#         -DCONFIG_TFM_ENABLE_FP=OFF \
#         -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=build/ns/bin
#
#   cmake --build build/ns --target tfm_ns --parallel $(nproc)
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

if (NOT DEFINED CONFIG_SPE_PATH OR NOT EXISTS ${CONFIG_SPE_PATH})
    message(FATAL_ERROR
        "CONFIG_SPE_PATH = ${CONFIG_SPE_PATH} is not defined or incorrect.\n"
        "Please provide the full path to TF-M SPE build artifacts:\n"
        "  -DCONFIG_SPE_PATH=<your_build_dir>/spe/api_ns")
endif()

if (NOT DEFINED TI_SDK_PATH)
    if (DEFINED ENV{TI_SDK_PATH})
        set(TI_SDK_PATH $ENV{TI_SDK_PATH})
    else()
        message(FATAL_ERROR
            "TI_SDK_PATH must be set:\n"
            "  -DTI_SDK_PATH=<path_to_msp_sdk>")
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH ${CONFIG_SPE_PATH}/cmake)

# Include platform-specific CPU and architecture flags for NS side
include(${CONFIG_SPE_PATH}/platform/cpuarch.cmake)

# Include common configs exported from TF-M SPE build
include(${CONFIG_SPE_PATH}/cmake/spe_config.cmake)

# Platform-specific config (optional)
include(${CONFIG_SPE_PATH}/platform/config.cmake OPTIONAL)

# Include toolchain file (must be before project())
include(${TFM_TOOLCHAIN_FILE})

project(tfm_ns LANGUAGES C ASM)

# Bring in platform_ns, tfm_api_ns, tfm_api_ns_tz, platform_region_defs, etc.
add_subdirectory(${CONFIG_SPE_PATH} ${CMAKE_BINARY_DIR}/spe_ns)

# Provide the bare-metal NS interface dispatcher (no RTOS required)
target_sources(tfm_api_ns
    PRIVATE
        ${CONFIG_SPE_PATH}/interface/src/os_wrapper/tfm_ns_interface_bare_metal.c
)

target_link_libraries(tfm_api_ns
    PRIVATE
        tfm_api_ns_tz
)

add_executable(tfm_ns)

target_sources(tfm_ns
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main_ns.c
        ${CMAKE_CURRENT_SOURCE_DIR}/syscalls_stub.c
)

target_link_libraries(tfm_ns
    PRIVATE
        tfm_api_ns
        platform_ns
)

# Apply the NS linker script exported by the SPE build
target_add_scatter_file(tfm_ns ${CONFIG_SPE_PATH}/platform/linker_scripts)

set_target_properties(tfm_ns
    PROPERTIES
        RUNTIME_OUTPUT_NAME "tfm_ns"
        SUFFIX ".axf"
)

message(STATUS "=== TF-M NS Application Configuration ===")
message(STATUS "CONFIG_SPE_PATH: ${CONFIG_SPE_PATH}")
message(STATUS "TI_SDK_PATH:     ${TI_SDK_PATH}")
message(STATUS "TFM_PLATFORM:    ${TFM_PLATFORM}")
message(STATUS "Toolchain:       ${CMAKE_C_COMPILER}")
message(STATUS "Output:          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tfm_ns.axf")
message(STATUS "==========================================")

# Generate binary from ELF
add_custom_command(
    TARGET tfm_ns
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY}
        ARGS -O binary ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tfm_ns.axf
                       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tfm_ns.bin
    COMMENT "Generating tfm_ns.bin from tfm_ns.axf"
)
