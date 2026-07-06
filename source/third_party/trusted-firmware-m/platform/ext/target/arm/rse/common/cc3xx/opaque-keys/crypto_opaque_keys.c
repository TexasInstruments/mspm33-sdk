/*
 * Copyright (c) 2025, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "cc3xx_opaque_keys.h"
#include "crypto_opaque_key_ids.h"
#include "kmu_drv.h"
#include "psa/crypto.h"

/**
 * @brief           Return the corresponding key size of the corresponding
 *                  opaque key.
 *
 * @param key_id    Opaque key ID.
 * @return size_t   The opaque key ID size in bytes.
 */
inline size_t cc3xx_get_opaque_key_buffer_size(mbedtls_svc_key_id_t key) {
    (void)key;
    return OPAQUE_KEY_SIZE;
}

/**
 * @brief           translate they key id of an opaque key to its corresponding
 *                  HW key ID. The latter makes sense to actual consumer of
 *                  they key.
 *
 * @param[in] mbedtls_svc_key_id_t      The opaque key ID.
 * @return uint32_t                     The HW key slot ID.
 */
inline uint32_t cc3xx_get_builtin_key(mbedtls_svc_key_id_t key) {
    psa_key_id_t key_id = MBEDTLS_SVC_KEY_ID_GET_KEY_ID(key);

    switch (key_id) {
    case PSA_OPAQUE_KEY_ID_KRTL:
        return KMU_HW_SLOT_KRTL;

    case PSA_OPAQUE_KEY_ID_HUK:
        return KMU_HW_SLOT_HUK;

    case PSA_OPAQUE_KEY_ID_GUK:
        return KMU_HW_SLOT_GUK;

    case PSA_OPAQUE_KEY_ID_KP_CM:
        return KMU_HW_SLOT_KP_CM;

    case PSA_OPAQUE_KEY_ID_KCE_CM:
        return KMU_HW_SLOT_KCE_CM;

    case PSA_OPAQUE_KEY_ID_KP_DM:
        return KMU_HW_SLOT_KP_DM;

    case PSA_OPAQUE_KEY_ID_KCE_DM:
        return KMU_HW_SLOT_KCE_DM;
    }

    if ((key_id >= PSA_OPAQUE_KEY_ID_USER_SLOT_MIN) && (key_id <= PSA_OPAQUE_KEY_ID_USER_SLOT_MAX)) {
        return KMU_USER_SLOT_MIN + (key_id - PSA_OPAQUE_KEY_ID_USER_SLOT_MIN);
    }

    return CC3XX_OPAQUE_KEY_ID_INVALID;
}

/**
 * @brief           translate they key id of a HW key to its corresponding
 *                  Opaque key ID.
 *
 * @param[in] hw_key_i              The HW key slot ID.
 * @return mbedtls_svc_key_id_t     The opaque key ID.
 */
inline mbedtls_svc_key_id_t cc3xx_get_opaque_key(uint32_t hw_key_id) {
    psa_key_id_t key_id = CC3XX_OPAQUE_KEY_ID_INVALID;

    switch (hw_key_id) {
    case KMU_HW_SLOT_KRTL:
        key_id = PSA_OPAQUE_KEY_ID_KRTL;
        break;
    case KMU_HW_SLOT_HUK:
        key_id = PSA_OPAQUE_KEY_ID_HUK;
        break;
    case KMU_HW_SLOT_GUK :
        key_id = PSA_OPAQUE_KEY_ID_GUK;
        break;
    case KMU_HW_SLOT_KP_CM :
        key_id = PSA_OPAQUE_KEY_ID_KP_CM;
        break;
    case KMU_HW_SLOT_KCE_CM :
        key_id = PSA_OPAQUE_KEY_ID_KCE_CM;
        break;
    case KMU_HW_SLOT_KP_DM :
        key_id = PSA_OPAQUE_KEY_ID_KP_DM;
        break;
    case KMU_HW_SLOT_KCE_DM :
        key_id = PSA_OPAQUE_KEY_ID_KCE_DM;
        break;
    }

    if ((hw_key_id >= KMU_USER_SLOT_MIN ) && (hw_key_id <= KMU_USER_SLOT_MAX)) {
        key_id = PSA_OPAQUE_KEY_ID_USER_SLOT_MIN + (hw_key_id - KMU_USER_SLOT_MIN);
    }

    return mbedtls_svc_key_id_make(0, key_id);
}

