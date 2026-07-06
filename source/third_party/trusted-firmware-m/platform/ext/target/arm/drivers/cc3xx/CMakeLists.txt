#-------------------------------------------------------------------------------
# SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# Default configuration of cc3xx repository
set(PSA_CRYPTO_DRIVER_PATH     "DOWNLOAD"         CACHE PATH   "Path to cc3xx Crypto driver (or DOWNLOAD to fetch automatically")
set(PSA_CRYPTO_DRIVER_NAME     "vendor/arm/cc3xx" CACHE STRING "PSA Crypto driver name")
set(PSA_CRYPTO_DRIVER_VERSION  "3a93566"          CACHE STRING "Revision of cc3xx to fetch")
fetch_remote_library(
    LIB_NAME                cc3xx
    LIB_SOURCE_PATH_VAR     PSA_CRYPTO_DRIVER_PATH
    LIB_PATCH_DIR           ${CMAKE_CURRENT_LIST_DIR}
    LIB_BASE_DIR            "${CMAKE_BINARY_DIR}/driver"
    FETCH_CONTENT_ARGS
        GIT_REPOSITORY      https://git.trustedfirmware.org/shared/tf-psa-crypto-drivers
        GIT_TAG             ${PSA_CRYPTO_DRIVER_VERSION}
        GIT_SHALLOW         FALSE
        GIT_PROGRESS        TRUE
)

set(CC3XX_DRIVER_PATH ${PSA_CRYPTO_DRIVER_PATH}/${PSA_CRYPTO_DRIVER_NAME} CACHE PATH "CC3XX Crypto driver path")
