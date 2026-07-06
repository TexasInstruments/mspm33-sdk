#-------------------------------------------------------------------------------
# SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

add_library(tfm_efi_guid STATIC EXCLUDE_FROM_ALL)

target_sources(tfm_efi_guid
    PRIVATE
        src/efi_guid.c
)

target_include_directories(tfm_efi_guid
    PUBLIC
        inc
)

target_link_libraries(tfm_efi_guid
    PUBLIC
        psa_interface
)
