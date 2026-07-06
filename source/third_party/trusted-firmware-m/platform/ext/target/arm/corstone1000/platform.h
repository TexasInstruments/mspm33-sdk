/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "efi_guid_structs.h"

#if !PLAT_GPT_MAX_PARTITIONS
#define PLAT_GPT_MAX_PARTITIONS 16
#endif /* PLAT_GPT_MAX_PARTITIONS */

#define FWU_METADATA_TYPE_UUID  \
    MAKE_EFI_GUID(              \
            0x8A7A84A0,         \
            0x8387,             \
            0x40F6,             \
            0xAB, 0x41,         \
            0xA8, 0xB9, 0xA5, 0xA6, 0x0D, 0x23)
#define PRIVATE_METADATA_TYPE_UUID  \
    MAKE_EFI_GUID(                  \
            0xECB55DC3,             \
            0x8AB7,                 \
            0x4A84,                 \
            0xAB, 0x56,             \
            0xEB, 0x0A, 0x99, 0x74, 0xDB, 0x42)

/* Initialize io storage of the platform */
int32_t plat_io_storage_init(void);

#endif /*__PLATFORM_H__*/
