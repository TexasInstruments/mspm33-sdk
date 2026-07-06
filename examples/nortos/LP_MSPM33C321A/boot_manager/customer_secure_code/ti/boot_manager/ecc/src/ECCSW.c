/********************************************************************************
 **                                                                            **
 **                      Copyright 2006 -- 2012 (c)  INVIA                     **
 **                                                                            **
 **   All rights reserved. Reproduction in whole or part is prohibited         **
 **      without the written permission of the copyright owner                 **
 **                                                                            **
 ********************************************************************************
 ** File Name:      ECCSW.c
 ** Release:        2.0
 ** Author:         Alexandre Berzati <alexandre.berzati@invia.fr> - INVIA
 **
 ** Description:    ECC API Implementation
 **
 ********************************************************************************
 */

/*
 * Customizations:
 *  - Removed global variables and modified APIs accordingly.
 *  - Modified to use stdint.
 *  - Added ECCSW_validatePublicKeyWeierstrass().
 *  - Added ECCSW_validatePrivateKeyWeierstrass().
 *  - Removed duplicated status codes.
 *
 * Copyright (c) 2021, Texas Instruments Incorporated
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

#include "ECCSW.h"
#include "lowlevelapi.h"
#include "scalarMul.h"

#include <stdint.h>
#include <string.h>

/******************************************************************************/
/***                               STATUS                                   ***/
/******************************************************************************/

/******************************************************************************/
/***                           GLOBAL VARIABLES                             ***/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/***                            ECC APIs                                    ***/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

// **************************************************************************
// *** ECCSW_ECDSASign() API
// **************************************************************************
//
// INPUTS:
//              secret key (x)
//              message digest (h)
//              random string (k)
// OUTPUTS:
//              signature part 1 (s1 = x-coord(k.G))
//              signature part 2 (s2 = (h + x.s1) k^(-1) mod r)
//
// NB: memory requirement (workzone) is = to SCALARMUL()
// **************************************************************************

uint8_t ECCSW_ECDSASign(ECC_State *state, uint32_t *ecdsaSign_Key,
    uint32_t *ecdsaSign_Digest, uint32_t *ecdsaSign_Random,
    uint32_t *ecdsaSign_Sig_P1, uint32_t *ecdsaSign_Sig_P2)
{
    uint32_t l;
    uint8_t ret;

    // *****************************************************************
    // Memory Allocation
    // *****************************************************************
    uint32_t *ECDSA_SIGN_MOD = state->workzone, *ECDSA_SIGN_TMP1,
             *ECDSA_SIGN_TMP2, *ECDSA_SIGN_TMP3, *ECDSA_SIGN_TMP4,
             *ECDSA_SIGN_TMP5, *ECDSA_SIGN_TMP6;

    // *****************************************************************
    // Step 1: Compute s1 = x-coord(k.G)
    // *****************************************************************

    state->inScalar = ecdsaSign_Random;
    state->inPointX = state->data_Gx;
    state->inPointY = state->data_Gy;

    state->outPointX = ecdsaSign_Sig_P1;
    state->outPointY = ecdsaSign_Sig_P2;

    ret = SCALARMUL(state);
    if (ret != STATUS_SCALAR_MUL_OK) return ret;

    // *****************************************************************
    // Step 2: Load curve order as modulus
    // *****************************************************************
    state->LEN = IMPORTMODULUS(ECDSA_SIGN_MOD, state->data_r);

    if (state->LEN == 0) return STATUS_ORDER_LENGTH_ZERO;
    if (state->LEN > 255) return STATUS_ORDER_LARGER_THAN_255_WORDS;

    // NB : non generic
    if ((ECDSA_SIGN_MOD[2] & 0x1) != 1) return STATUS_ORDER_EVEN;
    if (ECDSA_SIGN_MOD[state->LEN + 1] == 0) return STATUS_ORDER_MSW_IS_ZERO;

    ECDSA_SIGN_TMP1 = (uint32_t *) (ECDSA_SIGN_MOD + MODSIZE(state->LEN));
    ECDSA_SIGN_TMP2 = (uint32_t *) (ECDSA_SIGN_TMP1 + OPSIZE(state->LEN));
    ECDSA_SIGN_TMP3 = (uint32_t *) (ECDSA_SIGN_TMP2 + OPSIZE(state->LEN));
    ECDSA_SIGN_TMP4 = (uint32_t *) (ECDSA_SIGN_TMP3 + OPSIZE(state->LEN));
    ECDSA_SIGN_TMP5 = (uint32_t *) (ECDSA_SIGN_TMP4 + OPSIZE(state->LEN));
    ECDSA_SIGN_TMP6 = (uint32_t *) (ECDSA_SIGN_TMP5 + OPSIZE(state->LEN));

    // Compute the Montgomery constant

    mSET(state, ECDSA_SIGN_MOD, state->LEN);

    // *****************************************************************
    // Step 3: Compute 1/K^2 mod r (stored in TMP1)
    // *****************************************************************

    mINVK(state, ECDSA_SIGN_TMP1);

    mADD(ECDSA_SIGN_TMP2, ECDSA_SIGN_TMP1, ECDSA_SIGN_TMP1, state->MOD,
        state->LEN);

    state->aux64[0] = MONTLOG(state->LEN);
    COPY(ECDSA_SIGN_TMP1,
        SQUAREMULTIPLYWORD(
            state, ECDSA_SIGN_TMP3, ECDSA_SIGN_TMP2, ECDSA_SIGN_TMP1),
        OPSIZE(state->LEN));

    // *****************************************************************
    // Step 4: Compute k^(-1) mod r (stored in TMP3)
    // *****************************************************************

    IMPORTOPERAND(state, ECDSA_SIGN_TMP4, ecdsaSign_Random);

    // -> normalise

    mMULT(ECDSA_SIGN_TMP3, ECDSA_SIGN_TMP4, ECDSA_SIGN_TMP1, state->MOD,
        state->LEN);

    // Inversion

    state->aux64[0] = 3 * (1 << (state->win - 2)) + 4;

    COPY(ECDSA_SIGN_TMP3, INVERT(state, ECDSA_SIGN_TMP3), OPSIZE(state->LEN));

    // *****************************************************************
    // Step 6: Compute h + x.s1
    // *****************************************************************

    // Load and normalize s1 = x-coord(k.G) (stored in TMP4)

    IMPORTOPERAND(state, ECDSA_SIGN_TMP5, ecdsaSign_Sig_P1);

    mMULT(ECDSA_SIGN_TMP4, ECDSA_SIGN_TMP5, ECDSA_SIGN_TMP1, state->MOD,
        state->LEN);

    // Load, mask and normalize key x (stored in TMP5)

    // -> load

    l = IMPORTOPERAND(state, ECDSA_SIGN_TMP6, ecdsaSign_Key);

    if (l > state->LEN) return STATUS_ECC_KEY_TOO_LONG;
    if (l == 0) return STATUS_ECC_KEY_LENGTH_ZERO;

    // -> normalize

    mMULT(ECDSA_SIGN_TMP5, ECDSA_SIGN_TMP6, ECDSA_SIGN_TMP1, state->MOD,
        state->LEN);

    // Compute x.s1 (stored in TMP6)

    mMULT(ECDSA_SIGN_TMP6, ECDSA_SIGN_TMP4, ECDSA_SIGN_TMP5, state->MOD,
        state->LEN);

    // Load, mask and normalize digest h (stored in TMP4)

    // -> load
    l = IMPORTOPERAND(state, ECDSA_SIGN_TMP5, ecdsaSign_Digest);

    if (l > state->LEN) return STATUS_DIGEST_TOO_LONG;
    if (l == 0) return STATUS_DIGEST_LENGTH_ZERO;

    // -> normalize

    mMULT(ECDSA_SIGN_TMP4, ECDSA_SIGN_TMP5, ECDSA_SIGN_TMP1, state->MOD,
        state->LEN);

    // Compute h + x.s1 (stored in TMP5)
    mADD(ECDSA_SIGN_TMP5, ECDSA_SIGN_TMP4, ECDSA_SIGN_TMP6, state->MOD,
        state->LEN);

    // Compute s2 = (h + x.s1) k^(-1) (stored in TMP1)

    mMULT(ECDSA_SIGN_TMP1, ECDSA_SIGN_TMP5, ECDSA_SIGN_TMP3, state->MOD,
        state->LEN);

    // *****************************************************************
    // Step 7: Denormalize and export s2
    // *****************************************************************

    SETOPERAND(state, ECDSA_SIGN_TMP3, 1);

    mMULT(ECDSA_SIGN_TMP2, ECDSA_SIGN_TMP1, ECDSA_SIGN_TMP3, state->MOD,
        state->LEN);

    mOUT(ECDSA_SIGN_TMP1, ECDSA_SIGN_TMP2, state->MOD, state->LEN);

    EXPORTOPERAND(state, ecdsaSign_Sig_P2, ECDSA_SIGN_TMP1);

    return STATUS_ECDSA_SIGN_OK;
}

// **************************************************************************
// *** ECCSW_ECDSAVerify() API
// **************************************************************************
//
// INPUTS:
//              public key x-coord (Px)
//              public key y-coord (Py)
//              message digest (h)
//              signature part 1 (s1)
//              signature part 2 (s2)
//
// OUTPUTS:
//              STATUS_ECDSA_VALID_SIGNATURE iff signature is OK
//              STATUS_ECDSA_INVALID_SIGNATURE iff signature is not OK
//
// NB: memory requirement (workzone) is SCALARMUL() + 7 * LEN + 4
// **************************************************************************

uint8_t ECCSW_ECDSAVerify(ECC_State *state, uint32_t *ecdsaVerif_Pk_X,
    uint32_t *ecdsaVerif_Pk_Y, uint32_t *ecdsaVerif_Digest,
    uint32_t *ecdsaSig_P1, uint32_t *ecdsaSig_P2)
{
    uint8_t ret;
    uint32_t l;

    uint32_t *ECDSA_verif_workzone = state->workzone;

    // *****************************************************************
    // Memory Allocation
    // *****************************************************************
    uint32_t *ECDSA_VERIF_MOD = ECDSA_verif_workzone, *ECDSA_VERIF_TMP1,
             *ECDSA_VERIF_TMP2, *ECDSA_VERIF_TMP3, *ECDSA_VERIF_TMP4,
             *ECDSA_VERIF_TMP5, *ECDSA_VERIF_TMP6, *ECDSA_VERIF_TMP7,
             *ECDSA_VERIF_TMP8;

    // *****************************************************************
    // Step 1: Load curve order as modulus
    // *****************************************************************
    state->LEN = IMPORTMODULUS(ECDSA_VERIF_MOD, state->data_r);

    if (state->LEN == 0) return STATUS_ORDER_LENGTH_ZERO;
    if (state->LEN > 255) return STATUS_ORDER_LARGER_THAN_255_WORDS;

    // NB: non generic
    if ((ECDSA_VERIF_MOD[2] & 0x1) != 1) return STATUS_ORDER_EVEN;
    if (ECDSA_VERIF_MOD[state->LEN + 1] == 0) return STATUS_ORDER_MSW_IS_ZERO;

    ECDSA_VERIF_TMP1 = (uint32_t *) (ECDSA_VERIF_MOD + MODSIZE(state->LEN));
    ECDSA_VERIF_TMP2 =
        (uint32_t *) (ECDSA_VERIF_TMP1 + OPSIZE(state->LEN) + 1);
    ECDSA_VERIF_TMP3 =
        (uint32_t *) (ECDSA_VERIF_TMP2 + OPSIZE(state->LEN) + 1);
    ECDSA_VERIF_TMP4 = (uint32_t *) (ECDSA_VERIF_TMP3 + OPSIZE(state->LEN));
    ECDSA_VERIF_TMP5 = (uint32_t *) (ECDSA_VERIF_TMP4 + OPSIZE(state->LEN));
    ECDSA_VERIF_TMP6 = (uint32_t *) (ECDSA_VERIF_TMP5 + OPSIZE(state->LEN));
    ECDSA_VERIF_TMP7 = (uint32_t *) (ECDSA_VERIF_TMP6 + OPSIZE(state->LEN));
    ECDSA_VERIF_TMP8 = (uint32_t *) (ECDSA_VERIF_TMP7 + OPSIZE(state->LEN));
    // Compute the Montgomery constant

    mSET(state, ECDSA_VERIF_MOD, state->LEN);

    // *****************************************************************
    // Step 4: Compute 1/K^2 mod r (stored in TMP1)
    // *****************************************************************

    mINVK(state, ECDSA_VERIF_TMP1);

    mADD(ECDSA_VERIF_TMP2, ECDSA_VERIF_TMP1, ECDSA_VERIF_TMP1, state->MOD,
        state->LEN);

    state->aux64[0] = MONTLOG(state->LEN);

    COPY(ECDSA_VERIF_TMP1,
        SQUAREMULTIPLYWORD(
            state, ECDSA_VERIF_TMP3, ECDSA_VERIF_TMP2, ECDSA_VERIF_TMP1),
        OPSIZE(state->LEN));

    // *****************************************************************
    // Step 5: Compute s2^(-1) (stored in TMP3)
    // *****************************************************************

    // Load and normalize s2

    l = IMPORTOPERAND(state, ECDSA_VERIF_TMP3, ecdsaSig_P2);

    if (l > state->LEN) return STATUS_SIG_P2_TOO_LONG;
    if (l == 0) return STATUS_SIG_P2_LENGTH_ZERO;

    mMULT(ECDSA_VERIF_TMP2, ECDSA_VERIF_TMP3, ECDSA_VERIF_TMP1, state->MOD,
        state->LEN);

    // Inversion

    state->aux64[0] = 3 * (1 << (state->win - 2)) + 12;

    COPY(
        ECDSA_VERIF_TMP2, INVERT(state, ECDSA_VERIF_TMP2), OPSIZE(state->LEN));

    // Multiplication by 1/K^2 (to save one mult hereafter)

    mMULT(ECDSA_VERIF_TMP3, ECDSA_VERIF_TMP2, ECDSA_VERIF_TMP1, state->MOD,
        state->LEN);

    // *****************************************************************
    // Step 6: Compute u = s2^(-1).h and v = s2^(-1).s2
    //         (stored in TMP1 and TMP2)
    // *****************************************************************

    // Load digest h

    l = IMPORTOPERAND(state, ECDSA_VERIF_TMP4, ecdsaVerif_Digest);

    if (l > state->LEN) return STATUS_DIGEST_TOO_LONG;
    if (l == 0) return STATUS_DIGEST_LENGTH_ZERO;

    // Normalise h and compute u = s2^(-1).h

    mMULT(ECDSA_VERIF_TMP1, ECDSA_VERIF_TMP4, ECDSA_VERIF_TMP3, state->MOD,
        state->LEN);

    // Denormalise and reduce u

    SETOPERAND(state, ECDSA_VERIF_TMP4, 1);

    mMULT(ECDSA_VERIF_TMP5, ECDSA_VERIF_TMP1, ECDSA_VERIF_TMP4, state->MOD,
        state->LEN);

    ECDSA_VERIF_TMP1[0] = state->LEN;
    mOUT(ECDSA_VERIF_TMP1 + 1, ECDSA_VERIF_TMP5, state->MOD, state->LEN);

    // Load signature 1st part s1

    l = IMPORTOPERAND(state, ECDSA_VERIF_TMP4, ecdsaSig_P1);

    if (l > state->LEN) return STATUS_SIG_P1_TOO_LONG;
    if (l == 0) return STATUS_SIG_P1_LENGTH_ZERO;

    // Normalise s1 and compute v = s2^(-1).s1

    mMULT(ECDSA_VERIF_TMP2, ECDSA_VERIF_TMP4, ECDSA_VERIF_TMP3, state->MOD,
        state->LEN);

    // Denormalise and reduce v
    SETOPERAND(state, ECDSA_VERIF_TMP4, 1);

    mMULT(ECDSA_VERIF_TMP5, ECDSA_VERIF_TMP2, ECDSA_VERIF_TMP4, state->MOD,
        state->LEN);

    ECDSA_VERIF_TMP2[0] = state->LEN;
    mOUT(ECDSA_VERIF_TMP2 + 1, ECDSA_VERIF_TMP5, state->MOD, state->LEN);

    // *****************************************************************
    // Step 7: 1st scalar multiplication : u.G
    // *****************************************************************

    state->inPointX = state->data_Gx;
    state->inPointY = state->data_Gy;

    state->inScalar = ECDSA_VERIF_TMP1;
    state->workzone = ECDSA_VERIF_TMP8;

    // init scalar mult

    ret = SCALARMUL_init(state);
    if (ret != STATUS_SCALAR_MUL_OK) return ret;

    // save 1/K in TMP3

    COPY(ECDSA_VERIF_TMP3, REF_INVK, OPSIZE(state->LEN));

    // save 1/K^2 in TMP4

    COPY(ECDSA_VERIF_TMP4, REF_INVK2, OPSIZE(state->LEN));

    // core scalar mult

    ret = SCALARMUL_core(state);
    if (ret != STATUS_SCALAR_MUL_OK) return ret;

    // save Jacobian coordinates in TMP5, TMP6, TMP7

    SAVEPOINT(state, ECDSA_VERIF_TMP5);

    // *****************************************************************
    // Step 8: 2nd scalar multiplication : v.P
    // *****************************************************************

    state->inPointX = ecdsaVerif_Pk_X;
    state->inPointY = ecdsaVerif_Pk_Y;
    state->inScalar = ECDSA_VERIF_TMP2;

    // restore 1/K in TMP3

    COPY(REF_INVK, ECDSA_VERIF_TMP3, OPSIZE(state->LEN));

    // save 1/K^2 in TMP4

    COPY(REF_INVK2, ECDSA_VERIF_TMP4, OPSIZE(state->LEN));

    // core scalar mult

    ret = SCALARMUL_core(state);
    if (ret != STATUS_SCALAR_MUL_OK) return ret;

    // *****************************************************************
    // Step 9: Compute u.P + v .P
    // *****************************************************************

    state->aux64[0] = 2;
    JACADD(state, 0, ECDSA_VERIF_TMP5);

    state->outPointX = ECDSA_VERIF_TMP1;
    state->outPointY = ECDSA_VERIF_TMP2;
    SCALARMUL_final(state);

    state->workzone = ECDSA_verif_workzone;

    // *****************************************************************
    // Step 10: Compute ( x cordinate of (u.P + v .P) ) mod r
    // *****************************************************************

    l          = state->LEN + 1;
    state->LEN = IMPORTMODULUS(ECDSA_VERIF_MOD, state->data_r);

    mSET(state, ECDSA_VERIF_MOD, state->LEN);

    for (; l < state->LEN + 1; l++) ECDSA_VERIF_TMP1[l] = 0;

    ECDSA_VERIF_TMP2[0] = state->LEN;
    mOUT(ECDSA_VERIF_TMP2 + 1, ECDSA_VERIF_TMP1 + 1, state->MOD, state->LEN);

    // *****************************************************************
    // Step 11: Final comparison
    // *****************************************************************

    if (BUFFERSEQUAL(ECDSA_VERIF_TMP2, ecdsaSig_P1, state->LEN + 1)) {
        return STATUS_ECDSA_VALID_SIGNATURE;
    } else {
        return STATUS_ECDSA_INVALID_SIGNATURE;
    }
}

// **************************************************************************
// *** ECCSW_keyGen() API
// **************************************************************************
//
// INPUTS:
//              random string
//
// OUTPUTS:
//              private key
//              public key x-coord
//              public key y-coord
//
// NB: memory requirement (workzone) is equal to SCALARMUL()
// **************************************************************************

uint8_t ECCSW_keyGen(ECC_State *state, uint32_t *eccKeyGen_Random,
    uint32_t *eccKeyGen_Sk, uint32_t *eccKeyGen_Pk_X, uint32_t *eccKeyGen_Pk_Y)
{
    // Copy random string in private key buffer

    COPY(eccKeyGen_Sk, eccKeyGen_Random, eccKeyGen_Random[0] + 1);

    // Compute public key

    state->inScalar = eccKeyGen_Random;

    state->inPointX = state->data_Gx;
    state->inPointY = state->data_Gy;

    state->outPointX = eccKeyGen_Pk_X;
    state->outPointY = eccKeyGen_Pk_Y;

    return SCALARMUL(state);
}

// **************************************************************************
// *** ECCSW_ECDHCommonKey() API
// **************************************************************************
//
// INPUTS:
//              private key
//              ephemeral public key x-coord
//              ephemeral public key y-coord
//
// OUTPUTS:
//              common secret key x-ccord
//              common secret key y-ccord
//
// NB: memory requirement (workzone) is equal to SCALARMUL()
// **************************************************************************

uint8_t ECCSW_ECDHCommonKey(ECC_State *state, uint32_t *ecdhCommonKey_inScal,
    uint32_t *ecdhCommonKey_inX, uint32_t *ecdhCommonKey_inY,
    uint32_t *ecdhCommonKey_outX, uint32_t *ecdhCommonKey_outY)
{
    state->inScalar = ecdhCommonKey_inScal;
    state->inPointX = ecdhCommonKey_inX;
    state->inPointY = ecdhCommonKey_inY;

    state->outPointX = ecdhCommonKey_outX;
    state->outPointY = ecdhCommonKey_outY;

    return (SCALARMUL(state) - STATUS_SCALAR_MUL_OK) +
           STATUS_ECDH_COMMON_KEY_OK;
}

/*
 *  ======== ECCSW_validatePrivateKeyWeierstrass ========
 */
uint8_t ECCSW_validatePrivateKeyWeierstrass(
    ECC_State *state, const uint32_t *privateKey)
{
    uint32_t result;
    uint32_t len = IMPORTLENGTH(state->data_p);

    // Some operation require modulus form operators of 10 bytes length
    uint32_t modBufLen = len + 2;

    uint32_t *order;
    uint32_t *privKey;
    uint32_t *temp_buf;

    order    = state->workzone;
    privKey  = (order + modBufLen);
    temp_buf = (privKey + modBufLen);

    memset(state->workzone, 0x00, 3 * modBufLen * sizeof(uint32_t));

    // Import order to operand format form for range check
    IMPORTDATA(order, state->data_r);

    // Import private key to operand format form for range check
    IMPORTDATA(privKey, privateKey);

    // Verify private key != 0
    if (ISALLZEROS(privKey, len)) {
        return STATUS_PRIVATE_KEY_ZERO;
    }

    // Set state len since zSUB accesses it
    state->LEN = len;

    // Check private key range by subtracting privateKey-N,
    // there should be borrow and len'th byte should be set to zero
    result = zSUB(state, temp_buf, privKey, order);

    if ((result != 1) || (temp_buf[len] != 0)) {
        return STATUS_PRIVATE_KEY_LARGER_EQUAL_ORDER;
    }

    return STATUS_PRIVATE_VALID;
}

/*
 *  ======== ECCSW_validatePublicKeyWeierstrass ========
 */
uint8_t ECCSW_validatePublicKeyWeierstrass(
    ECC_State *state, const uint32_t *curvePointX, const uint32_t *curvePointY)
{
    uint32_t pkaResult;
    uint32_t len;

    // Some operation require modulus form operators of 10 bytes length
    uint32_t modBufLen = IMPORTLENGTH(state->data_p) + 2;

    uint32_t *primeMod;
    uint32_t *xImp_10Bt;
    uint32_t *yImp_10Bt;
    uint32_t *primeMod_10bt;
    uint32_t *temp_buff;
    uint32_t *temp_buff1;
    uint32_t *gx_mont;
    uint32_t *gy_mont;
    uint32_t *gx_hat;
    uint32_t *gy_hat;
    uint32_t *x2_hat;
    uint32_t *x3_hat;
    uint32_t *xa_hat;
    uint32_t *y2_hat;

    // Use allocated workzone instead of alllocating memory
    primeMod      = state->workzone;
    xImp_10Bt     = (primeMod + modBufLen);
    yImp_10Bt     = (xImp_10Bt + modBufLen);
    primeMod_10bt = (yImp_10Bt + modBufLen);
    gx_mont       = (primeMod_10bt + modBufLen);
    gy_mont       = (gx_mont + modBufLen);
    gx_hat        = (gy_mont + modBufLen);
    gy_hat        = (gx_hat + modBufLen);
    x2_hat        = (gy_hat + modBufLen);
    x3_hat        = (x2_hat + modBufLen);
    xa_hat        = (x3_hat + modBufLen);
    y2_hat        = (xa_hat + modBufLen);
    temp_buff     = (y2_hat + modBufLen);
    temp_buff1    = (temp_buff + modBufLen);

    memset(state->workzone, 0x00, 16 * modBufLen * sizeof(uint32_t));

    // Load curve order as modulus
    len = IMPORTMODULUS(primeMod, state->data_p);

    // Set prime mod
    mSET(state, primeMod, len);

    // Import  X point to operand format form for range check
    IMPORTDATA(xImp_10Bt, curvePointX);

    // Import  Y point to operand format form for range check
    IMPORTDATA(yImp_10Bt, curvePointY);

    // Import  P point to operand format form for range check
    IMPORTDATA(primeMod_10bt, state->data_p);

    // Verify X != 0 (not point at infinity)
    if (ISALLZEROS(xImp_10Bt, len)) {
        return STATUS_ECC_X_ZERO;
    }

    // Verify Y != 0 (not point at infinity)
    if (ISALLZEROS(yImp_10Bt, len)) {
        return STATUS_ECC_Y_ZERO;
    }

    // Check X point range by subtracting the X-P,
    // there should be borrow and len'th byte should be set to zero
    pkaResult = zSUB(state, temp_buff, xImp_10Bt, primeMod_10bt);

    if ((pkaResult != 1) || (temp_buff[len] != 0)) {
        return STATUS_ECC_X_LARGER_THAN_PRIME;
    }

    // Clear buffer
    memset(temp_buff, 0x00, (modBufLen * sizeof(uint32_t)));

    // Check Y point range Y <= [P -1]
    pkaResult = zSUB(state, temp_buff, yImp_10Bt, primeMod_10bt);

    if ((pkaResult != 1) || (temp_buff[len] != 0)) {
        return STATUS_ECC_Y_LARGER_THAN_PRIME;
    }

    // No need to compute the Montgomery constant
    // Convert point to  [X, Y] Montgomery format
    mMULT(gx_mont, xImp_10Bt, state->data_k_mont, state->MOD,
        state->LEN);  // Both X and Y points are in operand  format
    mMULT(gy_mont, yImp_10Bt, state->data_k_mont, state->MOD, state->LEN);

    // Compute x^2 hat
    mMULT(x2_hat, gx_mont, gx_mont, state->MOD, state->LEN);

    // Compute x^3 hat
    mMULT(x3_hat, x2_hat, gx_mont, state->MOD, state->LEN);

    // Compute xa_hat
    mMULT(xa_hat, gx_mont, state->data_a_mont, state->MOD, state->LEN);

    // Compute x3pax_hat
    memset(temp_buff, 0x00, sizeof(uint32_t) * 10);
    mADD(temp_buff, x3_hat, xa_hat, state->MOD, state->LEN);

    // Compute x3paxpb_hat
    mADD(temp_buff, temp_buff, state->data_b_mont, state->MOD, state->LEN);

    // Compute y2_hat
    mMULT(y2_hat, gy_mont, gy_mont, state->MOD, state->LEN);

    memset(temp_buff1, 0x00, modBufLen * sizeof(uint32_t));

    if (BUFFERSEQUAL(y2_hat, temp_buff, len) != true) {
        return STATUS_ECC_POINT_NOT_ON_CURVE;
    }

    return STATUS_ECC_POINT_ON_CURVE;
}
