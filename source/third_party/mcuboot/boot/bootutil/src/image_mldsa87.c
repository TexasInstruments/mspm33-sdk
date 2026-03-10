/*
 * Copyright (c) 2025, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* image_mldsa87.c */
#include <string.h>
#include "mcuboot_config/mcuboot_config.h"

#if defined(MCUBOOT_SIGN_MLDSA87_STANDARD) || defined(MCUBOOT_SIGN_MLDSA87_PREHASH) || defined(MCUBOOT_VERIFY_HYBRID)
#include "bootutil/sign_key.h"
#include "bootutil/crypto/sha256.h"
#include "bootutil_priv.h"
#include "bootutil/fault_injection_hardening.h"

/* Include your MLDSA implementation header */
#include "ti/crypto/include/pqc/ml-dsa.h"

/* MLDSA-specific verification function */
fih_int bootutil_verify_mldsa_sig(uint8_t *hash, uint32_t hlen, uint8_t *sig, size_t slen,
                                uint8_t key_id)
{
    int rc;
    fih_int fih_rc = FIH_FAILURE;
    
    /* Get the public key */
    const uint8_t *pub_key = bootutil_keys[key_id].key;
    const uint32_t pub_key_len = *bootutil_keys[key_id].len;
    
    /* Call the MLDSA-87 verification function with appropriate parameters */
    /* Note: Assuming no context is needed for this verification */
    const uint8_t *ctx = NULL;
    size_t ctxLen = 0;
    
    crypto_mldsa_mode_t hashMode = CRYPTO_MLDSA_SIG_MODE_PURE;
    
    rc = Crypto_MLDSA_87_verify(
        sig, slen,              /* Signature and its length */
        hash, hlen,             /* Message hash and its length */
        ctx, ctxLen,            /* Context (null in this case) */
        pub_key,                /* Public key */
        hashMode                /* Hash mode */
    );
    
    if (rc == CRYPTO_MLDSA_STAT_VALID_SIGNATURE) {
        FIH_CALL(fih_int_encode, fih_rc, 0);
    } else {
        fih_rc = fih_int_encode(rc);
    }
    
    FIH_RET(fih_rc);
}

/* Standard verification function for when MLDSA is the only algorithm */
#if (defined(MCUBOOT_SIGN_MLDSA87_STANDARD) || defined(MCUBOOT_SIGN_MLDSA87_PREHASH)) && !defined(MCUBOOT_VERIFY_HYBRID)
fih_int
bootutil_verify_sig(uint8_t *hash, uint32_t hlen, uint8_t *sig, size_t slen,
                 uint8_t key_id)
{
    return bootutil_verify_mldsa_sig(hash, hlen, sig, slen, key_id);
}
#endif

#endif /* MCUBOOT_SIGN_MLDSA87_STANDARD || MCUBOOT_SIGN_MLDSA87_PREHASH || MCUBOOT_VERIFY_HYBRID */