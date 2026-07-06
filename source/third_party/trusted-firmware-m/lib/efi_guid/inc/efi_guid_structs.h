/*
 * Copyright (c) 2021, Linaro Limited
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_EFI_GUID_STRUCTS_H__
#define __TFM_EFI_GUID_STRUCTS_H__

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EFI_GUID_NODE_LEN 6

/**
 * \brief Representation that makes creating GUIDs slightly easier.
 */
struct efi_guid_t {
    uint32_t time_low;                  /**< Low 32-bits of timestamp. */
    uint16_t time_mid;                  /**< Middle 16-bits of timestamp. */
    uint16_t time_hi_and_version;       /**< High 12-bits of timestamp with 4-bit version. */
    uint8_t clock_seq_hi_and_reserved;  /**< High 4-bits of clock sequence with 4-bit variant. */
    uint8_t clock_seq_low;              /**< Low 8-bits of clock sequence. */
    uint8_t node[EFI_GUID_NODE_LEN];    /**< 48-bit spatially unique node identifier. */
};

static inline int efi_guid_cmp(const struct efi_guid_t *g1,
                               const struct efi_guid_t *g2)
{
    return memcmp(g1, g2, sizeof(struct efi_guid_t));
}

static inline void *efi_guid_cpy(const struct efi_guid_t *src,
                                 struct efi_guid_t       *dst)
{
    return memcpy(dst, src, sizeof(struct efi_guid_t));
}

/** \brief Helper macro to build an EFI GUID structure from individual values. */
#define MAKE_EFI_GUID(a, b, c, d0, d1, e0, e1, e2, e3, e4, e5)  \
    {                                                           \
        (a) & 0xffffffff, (b)&0xffff, (c)&0xffff, d0, d1, {     \
            (e0), (e1), (e2), (e3), (e4), (e5)                  \
        }                                                       \
    }

/** \brief Helper macro to build the EFI GUID 00000000-0000-0000-0000-0000000000. */
#define NULL_GUID                                                           \
    MAKE_EFI_GUID(0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, \
            0x00, 0x00, 0x00)

#ifdef __cplusplus
}
#endif

#endif /* __TFM_EFI_GUID_STRUCTS_H__ */
