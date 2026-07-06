# CMake toolchain file for TI ARM Clang targeting Cortex-M33 (ARMv8-M)
# Compatible with TI MSP SDK build flags

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# TI ARM Clang compiler paths
# set(TI_TOOLCHAIN_PATH "")
if(DEFINED ENV{TI_TOOLCHAIN_PATH})
    set(TI_TOOLCHAIN_PATH "$ENV{TI_TOOLCHAIN_PATH}")
elseif(NOT TI_TOOLCHAIN_PATH)
    set(TI_TOOLCHAIN_PATH "")  # fallback
endif()

set(CMAKE_C_COMPILER "${TI_TOOLCHAIN_PATH}/bin/tiarmclang")
set(CMAKE_AR "${TI_TOOLCHAIN_PATH}/bin/tiarmar")

# tiarmar uses different syntax - use rcs instead of qc
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> rcs <TARGET> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> rcs <TARGET> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH "")

# Cortex-M33 specific flags (ARMv8-M Mainline with FPU)
set(CPU_FLAGS "-mcpu=cortex-m33 -march=armv8-m.main -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mlittle-endian")

# Include path for TI toolchain headers
set(TI_INCLUDE_FLAGS "-I${TI_TOOLCHAIN_PATH}/include")

# Compiler flags
set(CMAKE_C_FLAGS_INIT "${CPU_FLAGS} ${TI_INCLUDE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG_INIT "-Og -g")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O2")

# Disable features not available on bare-metal
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Don't look for programs on the host
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)