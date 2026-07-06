#-------------------------------------------------------------------------------
# SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

add_library(tfm_efi_soft_crc STATIC EXCLUDE_FROM_ALL)

target_sources(tfm_efi_soft_crc
    PRIVATE
        src/efi_soft_crc.c
)

target_include_directories(tfm_efi_soft_crc
    PUBLIC
        inc
)
