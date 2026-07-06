#-------------------------------------------------------------------------------
# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
# Copyright (c) 2025 Texas Instruments Incorporated
#
# This file is exported to NS side during CMake installation phase and renamed
# to CMakeLists.txt. It instructs how to build the platform_ns library for
# the LP_MSPM33C321A Non-Secure application.
#-------------------------------------------------------------------------------

add_library(platform_ns)

target_sources(platform_ns
    PRIVATE
        board.c
        clock_config.c
        pin_mux.c
        cmsis_drivers/Driver_USART.c
        Device/Source/startup_mspm33c321a_s.c
)

target_include_directories(platform_ns
    PUBLIC
        .
        Device/Include
        include
        partition
        cmsis_drivers
        ext/cmsis/Include
        ext/cmsis/Include/m-profile
        ext/common
)

target_compile_definitions(platform_ns
    PUBLIC
        __MSPM33C321A__
        MSPM33C321A
        __SAUREGION_PRESENT=1
        __DSP_PRESENT=1
        __FPU_PRESENT=0
        CMSIS_device_header="mspm33c321x.h"
)

# TI SDK path — set via -DTI_SDK_PATH=... or TI_SDK_PATH env var
if(NOT DEFINED TI_SDK_PATH)
    if(DEFINED ENV{TI_SDK_PATH})
        set(TI_SDK_PATH $ENV{TI_SDK_PATH})
    else()
        message(FATAL_ERROR "TI_SDK_PATH must be set (e.g. -DTI_SDK_PATH=/path/to/msp-sdk)")
    endif()
endif()

# TI SDK driverlib for NS side UART
target_sources(platform_ns
    PRIVATE
        ${TI_SDK_PATH}/source/ti/driverlib/dl_unicommuart.c
        ${TI_SDK_PATH}/source/ti/driverlib/dl_gpio.c
)

target_include_directories(platform_ns
    PUBLIC
        ${TI_SDK_PATH}/source
        ${TI_SDK_PATH}/source/ti/devices/msp/m33
        ${TI_SDK_PATH}/source/third_party/CMSIS/Core/Include
)

# Include region_defs.h and flash_layout.h
target_include_directories(platform_region_defs
    INTERFACE
        partition
)
