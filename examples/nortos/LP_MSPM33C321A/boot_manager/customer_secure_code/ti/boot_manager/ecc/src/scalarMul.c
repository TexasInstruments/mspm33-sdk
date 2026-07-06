/********************************************************************************
 **                                                                            **
 **                      Copyright 2006 -- 2012 (c)  INVIA                     **
 **                                                                            **
 **   All rights reserved. Reproduction in whole or part is prohibited         **
 **      without the written permission of the copyright owner                 **
 **                                                                            **
 ********************************************************************************
 ** File Name:      scalarMul.c
 ** Release:        2.0
 ** Author:         Alexandre Berzati <alexandre.berzati@invia.fr> - INVIA
 **
 ** Description:    Sources of main functions used for elliptic curve
 **                 computations
 **
 ********************************************************************************
 */

/*
 * Customizations:
 *  - Removed global variables and modified APIs accordingly.
 *  - Modified to use stdint.
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

#include "scalarMul.h"
#include "lowlevelapi.h"

#include <stdint.h>

/******************************************************************************/
/***                           INTERNAL MACRO                               ***/
/******************************************************************************/
#define NBACC ((uint8_t) 1 << (state->win - 2))

/******************************************************************************/
/***                           GLOBAL VARIABLES                             ***/
/******************************************************************************/

/******************************************************************************/
/***               Globals for Memory management                            ***/
/******************************************************************************/

/******************************************************************************/
/***                             CONSTANTS                                  ***/
/******************************************************************************/
const uint8_t LUTOPCODE[6] = {3, 3, 1, 2, 3, 3};

const uint8_t LUTOPSIGN[6][2][2] = {{{0, 0}, {0, 0}}, {{0, 1}, {1, 0}},
    {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};

const uint8_t LUTC[6][2][2] = {{{0, 1}, {0, 1}}, {{0, 1}, {0, 1}},
    {{0, 1}, {0, 1}}, {{0, 0}, {1, 1}}, {{0, 1}, {0, 1}}, {{0, 1}, {0, 1}}};

const uint8_t LUTSE[6][2][2] = {{{0, 1}, {0, 1}}, {{0, 0}, {1, 1}},
    {{0, 1}, {0, 1}}, {{0, 1}, {0, 1}}, {{0, 1}, {0, 1}}, {{0, 1}, {0, 1}}};

const uint8_t LUTINCI[6][4] = {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {2, 3, 4, 5}};

const uint8_t LUTJ[6][4][5] = {
    {{0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}},
    {{0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}},
    {{0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}},
    {{1, 1, 1, 1, 1}, {2, 2, 2, 2, 2}, {3, 3, 3, 3, 3}, {4, 4, 4, 4, 4}},
    {{0, 0, 1, 2, 3}, {0, 0, 1, 2, 3}, {0, 0, 1, 2, 3}, {0, 0, 1, 2, 3}},
    {{0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}}};

const uint8_t LUTSTATE[6][2][5] = {{{0, 0, 0, 0, 0}, {1, 1, 1, 1, 1}},
    {{2, 2, 2, 2, 2}, {2, 2, 2, 2, 2}}, {{3, 3, 3, 3, 3}, {3, 3, 3, 3, 3}},
    {{4, 5, 4, 4, 4}, {4, 5, 4, 4, 4}}, {{4, 5, 4, 4, 4}, {4, 5, 4, 4, 4}},
    {{0, 0, 0, 0, 0}, {1, 1, 1, 1, 1}}};

/******************************************************************************/
/***                       SUBFUNCTIONS DEFINITIONS                         ***/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/***                         SCALAR MULTIPLICATION                          ***/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

uint8_t SCALARMUL_init(ECC_State *state)
{
    uint32_t t;

    // *****************************************************************
    // Step 1 : Load modulus
    // *****************************************************************
    state->MOD = state->workzone;
    state->LEN = IMPORTMODULUS(state->MOD, state->data_p);

    if (state->LEN == 0) return STATUS_MODULUS_LENGTH_ZERO;
    if (state->LEN > 255) return STATUS_MODULUS_LARGER_THAN_255_WORDS;

    t = *(state->MOD + 2);
    t = t & 1;

    if (t != 1) return STATUS_MODULUS_EVEN;

    t = *(state->MOD + state->LEN + 1);

    if (t == 0) return STATUS_MODULUS_MSW_IS_ZERO;

    // *****************************************************************
    // Memory Allocation for Scalar Mod computation
    // *****************************************************************

    state->SCAL = (uint32_t *) (state->MOD + MODSIZE(state->LEN));
    state->EXPX = (uint32_t *) (state->SCAL + state->LEN + 2);
    state->EXPY = (uint32_t *) (state->EXPX + OPSIZE(state->LEN));
    state->EXPZ = (uint32_t *) (state->EXPY + OPSIZE(state->LEN));
    state->EXPW = (uint32_t *) (state->EXPZ + OPSIZE(state->LEN));

    state->ACCX = (uint32_t *) (state->EXPW + OPSIZE(state->LEN));
    state->ACCY = (uint32_t *) (state->ACCX + NBACC * OPSIZE(state->LEN));
    state->ACCZ = (uint32_t *) (state->ACCY + NBACC * OPSIZE(state->LEN));

    state->TMP1 = (uint32_t *) (state->ACCZ + NBACC * OPSIZE(state->LEN));
    state->TMP2 = (uint32_t *) (state->TMP1 + OPSIZE(state->LEN));
    state->TMP3 = (uint32_t *) (state->TMP2 + OPSIZE(state->LEN));
    state->TMP4 = (uint32_t *) (state->TMP3 + OPSIZE(state->LEN));
    state->TMP5 = (uint32_t *) (state->TMP4 + OPSIZE(state->LEN));
    state->TMP6 = (uint32_t *) (state->TMP5 + OPSIZE(state->LEN));

    // *****************************************************************
    // Step 2 : Montgomery precomputation
    // *****************************************************************

    // Compute -p^(-1) mod 2^32

    mSET(state, state->MOD, state->LEN);

    // Compute 1/K mod m (stored in EXPZ)

    mINVK(state, state->EXPZ);

    // Compute 2/K mod m (stored in TMP1)

    mADD(state->TMP1, state->EXPZ, state->EXPZ, state->MOD, state->LEN);

    // Compute 1/K^2 mod m (stored in TMP2)

    state->aux64[0] = MONTLOG(state->LEN);
    COPY(state->TMP2,
        SQUAREMULTIPLYWORD(state, state->TMP3, state->TMP1, state->TMP2),
        OPSIZE(state->LEN));

    return STATUS_SCALAR_MUL_OK;
}

uint8_t SCALARMUL_core(ECC_State *state)
{
    int i, j;
    uint8_t b, d, c, se, sd, opCode, opSign, STATE;
    uint32_t *ptr_tmp, t, l;

    // *****************************************************************
    // Step 3 : Load input scalar
    // *****************************************************************

    t = IMPORTOPERAND(state, state->SCAL, state->inScalar);

    if (t > (state->LEN + 1)) return STATUS_SCALAR_TOO_LONG;
    if (t == 0) return STATUS_SCALAR_LENGTH_ZERO;

    // Temporarily store the order r at SCAL+LEN+2
    ptr_tmp = (uint32_t *) (state->SCAL + state->LEN + 2);
    ZERO(ptr_tmp, state->LEN + 2);

    l = t = IMPORTDATA(ptr_tmp, state->data_r);

    if (t == 0) return STATUS_ORDER_LENGTH_ZERO;
    if (t > (state->LEN + 1)) return STATUS_ORDER_TOO_LONG;

    // Check window size

    if ((state->win < 2) || (state->win > 5)) return STATUS_BAD_WINDOW_SIZE;

    // Compute k' = k - 2^(2(w-2))+ t*r where t is a
    // 32-bit random s.t. k' is positive and even

    b = (*state->SCAL) & 1;

    t = 0;
    if (state->win > 2)
        t = (t & 0xFFFFFFFE) + b;
    else
        t = (t & 0xFFFFFFFE) + (1 - b);

    state->aux64[0] = t;
    t               = state->LEN;
    state->LEN      = l;
    zMACC32(state, state->SCAL, ptr_tmp, state->SCAL);

    state->aux64[0] = 1 << (2 * (state->win - 2));
    zSUB32(state, state->SCAL, state->SCAL);
    state->LEN = t;

    // *****************************************************************
    // Step 4 : Randomize and normalize input point
    // *****************************************************************

    // Set Z-coordinate to 1
    SETOPERAND(state, state->EXPZ, 1);

    // Compute Z^2/K^2 (stored in TMP4)

    mMULT(state->TMP3, state->EXPZ, state->TMP2, state->MOD, state->LEN);

    mMULT(state->TMP4, state->EXPZ, state->TMP3, state->MOD, state->LEN);

    // Compute Z^3/K^2 (stored in TMP3)

    mMULT(state->TMP3, state->EXPZ, state->TMP4, state->MOD, state->LEN);

    // Load, randomize and normalize X-coordinate
    // Load X-coordinate

    t = IMPORTOPERAND(state, state->TMP5, state->inPointX);

    if (t > state->LEN) return STATUS_X_COORD_TOO_LONG;
    if (t == 0) return STATUS_X_COORD_LENGTH_ZERO;

    // Compute X.Z^2/K

    mMULT(state->EXPX, state->TMP5, state->TMP4, state->MOD, state->LEN);

    // Load, randomize and normalize Y-coordinate
    // Load Y-coordinate
    t = IMPORTOPERAND(state, state->TMP5, state->inPointY);

    if (t > state->LEN) return STATUS_Y_COORD_TOO_LONG;
    if (t == 0) return STATUS_Y_COORD_LENGTH_ZERO;

    // Compute Y.Z^3/K

    mMULT(state->EXPY, state->TMP5, state->TMP3, state->MOD, state->LEN);

    // Compute normalized W-coordinate (= a.Z^4/K)
    // Load a

    IMPORTOPERAND(state, state->TMP4, state->data_a);

    if (t > state->LEN) return STATUS_A_COEF_TOO_LONG;
    if (t == 0) return STATUS_A_COEF_LENGTH_ZERO;

    // Compute a.Z^3/K

    mMULT(state->TMP5, state->TMP4, state->TMP3, state->MOD, state->LEN);

    // Compute a.Z^4/K

    mMULT(state->EXPW, state->TMP5, state->EXPZ, state->MOD, state->LEN);

    // *****************************************************************
    // Step 5 : Copy normalized point in accumulators
    // *****************************************************************

    state->aux64[0] = OPSIZE(state->LEN);
    ptr_tmp         = state->ACCX;
    for (i = 0; i < NBACC; i++) {
        COPY(ptr_tmp, state->EXPX, state->aux64[0]);
        ptr_tmp += state->aux64[0];
    }

    ptr_tmp = state->ACCY;
    for (i = 0; i < NBACC; i++) {
        COPY(ptr_tmp, state->EXPY, state->aux64[0]);
        ptr_tmp += state->aux64[0];
    }

    ptr_tmp = state->ACCZ;
    for (i = 0; i < NBACC; i++) {
        COPY(ptr_tmp, state->EXPZ, state->aux64[0]);
        ptr_tmp += state->aux64[0];
    }

    // *****************************************************************
    // Step 6 : Main loop
    // *****************************************************************

    c  = 0;
    se = 0;
    sd = 0;
    i  = 1;
    b  = 0;
    j  = 0;
    d  = 1;

    STATE = GETBIT(state->SCAL, (uint32_t *) 1);

    while (i < ((l * 32) + 1)) {
        state->aux64[0] =
            (state->win < (l * 32 + 1 - i)) ? state->win : (l * 32 + 1 - i);

        d               = (GETDIGIT(state, state->SCAL, (uint32_t *) i)) + c;
        sd              = (d < (1 << (state->win - 1))) ? 0 : 1;
        d               = sd * (1 << state->win) + (1 - (2 * sd)) * d;
        opCode          = LUTOPCODE[STATE];
        opSign          = LUTOPSIGN[STATE][se][sd];
        state->aux64[0] = opCode + 4 * opSign + 8 * ((d - 1) >> 1);

        ATOMICPATTERN(state);

        c  = LUTC[STATE][sd][c];
        se = LUTSE[STATE][sd][se];
        i += LUTINCI[STATE][state->win - 2];
        j = LUTJ[STATE][state->win - 2][j];

        b     = GETBIT(state->SCAL, (uint32_t *) i);
        STATE = LUTSTATE[STATE][b ^ c][j];
    }

    // Test if final carry is active

    if (c == 1) {
        // Jacobian doubling
        // -> Set exponentiator sign to +

        state->aux64[0] = 3 + 4 * se;

        ATOMICPATTERN(state);

        // Jacobian addition
        // -> Add exponentiator to first accumulator

        state->aux64[0] = 1;

        ATOMICPATTERN(state);

        state->aux64[0] = 2;

        ATOMICPATTERN(state);
    }

    // *****************************************************************
    // Step 7 : Aggregation
    // *****************************************************************

    // The result is stored in the first accumulator
    // If w=2 then no aggregation needed

    if (state->win > 2) {
        d = NBACC - 2;

        while (d > 0) {
            state->aux64[0] = 1;
            JACADD(state, (uint32_t *) ((uint32_t) d),
                (uint32_t *) ((uint32_t) d + 1));

            state->aux64[0] = 1;
            JACADD(
                state, (uint32_t *) (NBACC - 1), (uint32_t *) ((uint32_t) d));

            d = d - 1;
        }

        state->aux64[0] = 1;
        JACADD(state, 0, (uint32_t *) (NBACC - 1));

        state->aux64[0] = 1;
        JACADD(state, 0, (uint32_t *) (NBACC - 1));

        state->aux64[0] = 1;
        JACADD(state, 0, (uint32_t *) 1);
    }

    return STATUS_SCALAR_MUL_OK;
}

void SCALARMUL_final(ECC_State *state)
{
    // *****************************************************************
    // Memory Allocation for Scalar Mod computation
    // *****************************************************************
    uint32_t *TMPX, *TMPY, *TMPZ, *RESX, *RESY;

    TMPX = state->SCAL;
    TMPY = (uint32_t *) (state->SCAL + 1 + OPSIZE(state->LEN));
    TMPZ = (uint32_t *) (state->SCAL + 1 + 2 * OPSIZE(state->LEN));

    RESX = TMPY;
    RESY = TMPZ;

    // Copy result at begining of workzone after the modulus

    state->aux64[0] = OPSIZE(state->LEN);

    COPY(TMPX, state->ACCX, state->aux64[0]);

    COPY(TMPY, state->ACCY, state->aux64[0]);

    COPY(TMPZ, state->ACCZ, state->aux64[0]);

    // *****************************************************************
    // Step 8 : Invert Z-coord
    // *****************************************************************
    state->aux64[0] = 3 * NBACC + 7;

    INVERT(state, TMPZ);

    // *****************************************************************
    // Step 9 : Recover affine coordinates
    // *****************************************************************

    // Compute Z^(-2) and store it in TMP2

    mMULT(state->TMP2, TMPZ, TMPZ, state->MOD, state->LEN);

    // Compute Z^(-3) and store it in TMP1

    mMULT(state->TMP1, TMPZ, state->TMP2, state->MOD, state->LEN);

    // Recover affine coordinates

    mMULT(RESY, TMPY, state->TMP1, state->MOD, state->LEN);

    mMULT(RESX, TMPX, state->TMP2, state->MOD, state->LEN);

    // *****************************************************************
    // Step 10 : Compute (y^2 - x^3 - a.x) and load b for FA check
    // *****************************************************************

    // reload modulus fo FA check

    IMPORTMODULUS(state->MOD, state->data_p);

    mSET(state, state->MOD, state->LEN);
    // TMP 5 is used for denormalization
    SETOPERAND(state, state->TMP5, 1);

    // denormalize and output point
    // X-coord
    // denormalize

    mMULT(state->TMP3, RESX, state->TMP5, state->MOD, state->LEN);

    // output

    mOUT(RESX, state->TMP3, state->MOD, state->LEN);

    EXPORTOPERAND(state, state->outPointX, RESX);

    // Y-coord
    // denormalize

    mMULT(state->TMP4, RESY, state->TMP5, state->MOD, state->LEN);

    // output

    mOUT(RESY, state->TMP4, state->MOD, state->LEN);

    EXPORTOPERAND(state, state->outPointY, RESY);
}

uint8_t SCALARMUL(ECC_State *state)
{
    uint8_t ret;
    ret = SCALARMUL_init(state);
    if (ret != STATUS_SCALAR_MUL_OK) return ret;

    ret = SCALARMUL_core(state);
    if (ret != STATUS_SCALAR_MUL_OK) return ret;

    SCALARMUL_final(state);

    return STATUS_SCALAR_MUL_OK;
}

void SAVEPOINT(ECC_State *state, uint32_t *out)
{
    state->aux64[0] = OPSIZE(state->LEN);

    COPY(out, state->ACCX, state->aux64[0]);

    out += state->aux64[0];
    COPY(out, state->ACCY, state->aux64[0]);

    out += state->aux64[0];
    COPY(out, state->ACCZ, state->aux64[0]);
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/***                            SUBFUNCTIONS                                ***/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/***                          DATA MANIPULATION                              ***/
/******************************************************************************/

// GETBIT primitive
// -> extract the bit of given index
// INPUT: in1 - input address
//        in2 - index of the bit

uint8_t GETBIT(uint32_t *in1, uint32_t *in2)
{
    return (in1[((uint32_t) in2) >> 5] >> (((uint32_t) in2) & 0x1F)) & 1;
}

// GETDIGIT primitive
// -> extract the digit of given index (LSB) and bit-length from a given input
// INPUT: in1 - input address
//        in2 - index of the digit LSB
//        aux - bit-length of the digit

uint8_t GETDIGIT(ECC_State *state, uint32_t *in1, uint32_t *in2)
{
    uint8_t indWord, indBit;
    uint32_t t0, t1;

    indWord = ((uint32_t) in2) >> 5;
    indBit  = ((uint32_t) in2) & 0x1F;
    t0      = in1[indWord];
    t0      = t0 >> (indBit);
    t0      = t0 & ((1 << state->aux64[0]) - 1);
    if ((indBit + state->aux64[0]) > 32) {
        t1 = state->SCAL[indWord + 1];
        t1 = t1 & (((1 << (state->aux64[0] + indBit - 32)) - 1));
        t1 = t1 * (1 << (32 - indBit));
        t0 = t0 + t1;
    }

    return t0;
}

// GETDIGITL2R primitive
// -> extract the maximum odd digit with MSB at a given index and given maximum bit-length from a given input
// INPUT: in1 - input address
//        in2 - index of the digit MSB
//        aux - maximum bit-length of the digit

uint8_t GETDIGITL2R(ECC_State *state, uint32_t *in1, uint32_t *in2)
{
    uint8_t indWord, indBit;
    uint32_t t0, t1;

    if (((uint32_t) in2) < state->aux64[0]) {
        t0              = in1[0] & ((1 << ((uint32_t) in2 + 1)) - 1);
        state->aux64[0] = ((uint32_t) in2) + 1;
    }

    else {
        indWord = (((uint32_t) in2) + 1 - state->aux64[0]) >> 5;
        indBit  = (((uint32_t) in2) + 1 - state->aux64[0]) & 0x1F;

        t0 = in1[indWord];
        t0 = t0 >> indBit;
        t0 = t0 & ((1 << state->aux64[0]) - 1);

        if (indBit + state->aux64[0] > 32) {
            t1 = in1[indWord + 1];
            t1 = t1 & ((1 << (((uint32_t) in2 + 1) & 0x1F)) - 1);
            t1 = t1 * (1 << (state->aux64[0] - (((uint32_t) in2 + 1) & 0x1F)));
            t0 = t0 + t1;
        }
    }

    while ((t0 & 0x1) == 0) {
        t0 = t0 / 2;
        state->aux64[0]--;
    }

    return t0;
}

/******************************************************************************/
/***                          POINT OPEATIONS                               ***/
/******************************************************************************/
//uint32_t* SelectReg(uint8_t opCode, uint32_t* ptr1, uint32_t* ptr2, uint32_t* ptr3)
//{
//       return ((uint32_t*) (((((uint32_t) ptr1) ^
//           ((uint32_t) ptr2) ^ ((uint32_t) ptr3)) & 0xa5a5a5a5) ^
//           (((uint32_t) ptr1) & LUTSELCT1[opCode-1]) ^
//           (((uint32_t) ptr2) & LUTSELCT2[opCode-1]) ^
//           (((uint32_t) ptr3) & LUTSELCT3[opCode-1])));
//}
uint32_t *SelectReg(ECC_State *state, uint32_t opCode, uint32_t *ptr1,
    uint32_t *ptr2, uint32_t *ptr3)
{
    uint32_t *temp[4];

    temp[1] = ptr1;
    temp[2] = ptr2;
    temp[3] = ptr3;

    return temp[opCode];
}

void ATOMICPATTERN(ECC_State *state)
{
    uint32_t *ptr_tmp;
    uint8_t opCode, sign, indAcc;

    // Parse aux

    opCode = state->aux64[0] & 3;
    sign   = (state->aux64[0] >> 2) & 1;
    indAcc = (state->aux64[0] >> 3) * OPSIZE(state->LEN);

    // R1 = R2^2
    ptr_tmp = SelectReg(
        state, opCode, state->ACCZ + indAcc, state->TMP6, state->EXPX);
    mMULT(state->TMP1, ptr_tmp, ptr_tmp, state->MOD, state->LEN);

    // R3 = R4 + R5

    mADD(SelectReg(state, opCode, state->TMP2, state->TMP4, state->TMP2),
        state->EXPY, state->EXPY, state->MOD, state->LEN);

    // R6 = R7 * R8
    //    -> tmp = R7 * R8

    mMULT(state->TMP4,
        SelectReg(state, opCode, state->EXPY, state->TMP5, state->TMP2),
        SelectReg(
            state, opCode, state->ACCZ + indAcc, state->TMP1, state->EXPZ),
        state->MOD, state->LEN);

    //    -> R6 = tmp

    COPY(SelectReg(state, opCode, state->TMP2, state->TMP4, state->EXPZ),
        state->TMP4, OPSIZE(state->LEN));

    // R9 = R10 + R11

    mADD(SelectReg(state, opCode, state->TMP4, state->TMP5, state->TMP4),
        state->TMP1, state->TMP1, state->MOD, state->LEN);

    // R12 = R13 * R14

    mMULT(SelectReg(state, opCode, state->TMP5, state->TMP5, state->TMP3),
        SelectReg(
            state, opCode, state->ACCY + indAcc, state->TMP1, state->TMP2),
        SelectReg(state, opCode, state->EXPZ, state->TMP6, state->EXPY),
        state->MOD, state->LEN);

    // R15 = R16 + R17

    mADD(SelectReg(state, opCode, state->TMP6, state->TMP1, state->TMP6),
        state->TMP3, state->TMP3, state->MOD, state->LEN);

    // R18 = R19 * R20

    mMULT(SelectReg(state, opCode, state->TMP3, state->TMP1, state->TMP2),
        SelectReg(state, opCode, state->TMP1, state->EXPZ, state->TMP6),
        SelectReg(state, opCode, state->TMP2, state->TMP6, state->TMP3),
        state->MOD, state->LEN);

    // R21 = R22 + R23
    ptr_tmp = SelectReg(state, opCode, state->TMP2, state->TMP6, state->TMP1);
    mADD(ptr_tmp, state->TMP4, state->TMP1, state->MOD, state->LEN);

    // R24 = R25 + R26

    mADD(ptr_tmp, state->TMP1, state->EXPW, state->MOD, state->LEN);

    // R27 = R28^2

    ptr_tmp = SelectReg(state, opCode, state->EXPZ, state->TMP2, state->TMP1);

    mMULT(SelectReg(state, opCode, state->TMP4, state->TMP6, state->TMP3),
        ptr_tmp, ptr_tmp, state->MOD, state->LEN);

    // R29 = R30 * R31
    //    -> tmp = R30 * R31

    ptr_tmp = SelectReg(
        state, opCode, state->TMP2, state->ACCY + indAcc, state->TMP4);
    mMULT(ptr_tmp,
        SelectReg(state, opCode, state->TMP5, state->TMP1, state->TMP6),
        SelectReg(
            state, opCode, state->TMP4, state->ACCZ + indAcc, state->EXPX),
        state->MOD, state->LEN);

    //    -> R29 = tmp
    COPY(SelectReg(
             state, opCode, state->TMP2, state->ACCZ + indAcc, state->TMP4),
        ptr_tmp, OPSIZE(state->LEN));

    // R32 = R33 + R34
    ptr_tmp = SelectReg(state, opCode, state->EXPW, state->TMP4, state->EXPW);

    mADD(SelectReg(state, opCode, state->TMP5, state->TMP1, state->TMP5),
        ptr_tmp, ptr_tmp, state->MOD, state->LEN);

    // R35 = R36 - R37
    ptr_tmp = SelectReg(state, opCode, state->TMP2, state->TMP6, state->TMP3);
    mSUB(ptr_tmp, ptr_tmp,
        SelectReg(state, opCode, state->TMP3, state->TMP1, state->TMP4),
        state->MOD, state->LEN);

    // R38 = R39 * R40

    mMULT(SelectReg(state, opCode, state->TMP5, state->TMP1, state->EXPW),
        SelectReg(state, opCode, state->TMP1, state->TMP5, state->TMP2),
        SelectReg(state, opCode, state->EXPX, state->TMP3, state->TMP5),
        state->MOD, state->LEN);

    // R41 = R42 - R43

    mSUB(SelectReg(
             state, opCode, state->TMP6, state->ACCX + indAcc, state->EXPX),
        SelectReg(state, opCode, state->TMP3, state->TMP6, state->TMP3),
        SelectReg(state, opCode, state->TMP4, state->TMP5, state->TMP4),
        state->MOD, state->LEN);

    // R44 = R45 - R46

    mSUB(SelectReg(state, opCode, state->TMP6, state->TMP4, state->TMP6),
        state->TMP4,
        SelectReg(
            state, opCode, state->EXPX, state->ACCX + indAcc, state->EXPX),
        state->MOD, state->LEN);

    // R47 = R48 * R49

    mMULT(SelectReg(state, opCode, state->TMP6, state->TMP3, state->TMP4),
        SelectReg(
            state, opCode, state->ACCX + indAcc, state->TMP4, state->TMP6),
        SelectReg(state, opCode, state->TMP4, state->TMP2, state->TMP1),
        state->MOD, state->LEN);

    // R50 = R51 - R52
    mSUB(SelectReg(
             state, opCode, state->TMP6, state->ACCY + indAcc, state->EXPY),
        SelectReg(state, opCode, state->TMP6, state->TMP3,
            SelectReg(state, 1 + sign, state->TMP4, state->TMP2, state->TMP6)),
        SelectReg(state, opCode, state->TMP5, state->TMP1,
            SelectReg(state, 1 + sign, state->TMP2, state->TMP4, state->TMP6)),
        state->MOD, state->LEN);
}

// JACADD primitive
//  -> If aux=0, then perform a Jacobian addition of the exponentiator to the i-th accumulator
//  -> If aux=1, then perform a Jacobian addition of the j-th accumulator to the i-th accumulator
//  -> If aux=2, then perform a Jacobian addition of the point at address in2 to the i-th accumulator
// INPUT: in1 - index of the accumulator (i)
//        in2 - index of the accumulator (j) / address of the point to add
//        aux - mode in {0,1,2}

void JACADD(ECC_State *state, uint32_t *in1, uint32_t *in2)
{
    uint32_t accInd;
    uint32_t *X2 = 0, *Y2 = 0, *Z2 = 0, *ptr_tmp;

    accInd = ((uint32_t) in2) * OPSIZE(state->LEN);

    switch (state->aux64[0]) {
        case 0:
            // mode 0

            X2 = state->EXPX;
            Y2 = state->EXPY;
            Z2 = state->EXPZ;
            break;

        case 1:
            // mode 1
            X2 = state->ACCX + accInd;
            Y2 = state->ACCY + accInd;
            Z2 = state->ACCZ + accInd;
            break;

        case 2:
            // mode 2

            X2 = in2;
            Y2 = in2 + OPSIZE(state->LEN);
            Z2 = Y2 + OPSIZE(state->LEN);
            break;
    }

    // recompute accInd
    accInd = ((uint32_t) in1) * OPSIZE(state->LEN);

    // TMP4=Z1^2

    ptr_tmp = state->ACCZ + accInd;

    mMULT(state->TMP4, ptr_tmp, ptr_tmp, state->MOD, state->LEN);

    // TMP1=X2*TMP4

    mMULT(state->TMP1, X2, state->TMP4, state->MOD, state->LEN);

    // TMP5=Y2*Z1

    mMULT(state->TMP5, Y2, state->ACCZ + accInd, state->MOD, state->LEN);

    // TMP2=TMP5*TMP4

    mMULT(state->TMP2, state->TMP5, state->TMP4, state->MOD, state->LEN);

    // TMP4=Z2^2

    mMULT(state->TMP4, Z2, Z2, state->MOD, state->LEN);

    // TMP5=X1*TMP4

    mMULT(state->TMP5, state->ACCX + accInd, state->TMP4, state->MOD,
        state->LEN);

    // X1=Y1*Z2

    mMULT(state->ACCX + accInd, state->ACCY + accInd, Z2, state->MOD,
        state->LEN);

    // Y1=X1*TMP4

    mMULT(state->ACCY + accInd, state->ACCX + accInd, state->TMP4, state->MOD,
        state->LEN);

    // X1=TMP5-TMP1

    mSUB(state->ACCX + accInd, state->TMP5, state->TMP1, state->MOD,
        state->LEN);

    // TMP5=Z2*Z1

    mMULT(state->TMP5, Z2, state->ACCZ + accInd, state->MOD, state->LEN);

    // Z1=X1*TMP5

    mMULT(state->ACCZ + accInd, state->ACCX + accInd, state->TMP5, state->MOD,
        state->LEN);

    // TMP5=Y1-TMP2

    mSUB(state->TMP5, state->ACCY + accInd, state->TMP2, state->MOD,
        state->LEN);

    // TMP4=X1^2

    ptr_tmp = state->ACCX + accInd;

    mMULT(state->TMP4, ptr_tmp, ptr_tmp, state->MOD, state->LEN);

    // TMP3=TMP5^2

    mMULT(state->TMP3, state->TMP5, state->TMP5, state->MOD, state->LEN);

    // Y1=TMP1*TMP4

    mMULT(state->ACCY + accInd, state->TMP1, state->TMP4, state->MOD,
        state->LEN);

    // TMP1=TMP4*X1

    mMULT(state->TMP1, state->TMP4, state->ACCX + accInd, state->MOD,
        state->LEN);

    // TMP3=TMP3-TMP1

    mSUB(state->TMP3, state->TMP3, state->TMP1, state->MOD, state->LEN);

    // TMP4=Y1+Y1

    mADD(state->TMP4, state->ACCY + accInd, state->ACCY + accInd, state->MOD,
        state->LEN);

    // X1=TMP3-TMP4

    mSUB(state->ACCX + accInd, state->TMP3, state->TMP4, state->MOD,
        state->LEN);

    // TMP3=Y1-X1

    mSUB(state->TMP3, state->ACCY + accInd, state->ACCX + accInd, state->MOD,
        state->LEN);

    // TMP6=TMP5*TMP3

    mMULT(state->TMP6, state->TMP5, state->TMP3, state->MOD, state->LEN);

    // TMP4=TMP2*TMP1

    mMULT(state->TMP4, state->TMP2, state->TMP1, state->MOD, state->LEN);

    // Y1=TMP6-TMP4

    mSUB(state->ACCY + accInd, state->TMP6, state->TMP4, state->MOD,
        state->LEN);
}

/******************************************************************************/
/***                          MODULAR INVERSION                             ***/
/******************************************************************************/

// INVERT primitive
// -> Perform a modular inversion of the element at address in in1
// INPUT: in1 - address of the value to invert
//        aux - available memory at adress in1
//              (in number of buffers of lenght OPSIZE(LEN))
// OUTPUT: the result is written at input address (in1)

uint32_t *INVERT(ECC_State *state, uint32_t *in1)
{
    uint8_t winInv = 0, digit, carry, j;
    short int i;
    uint32_t tmp;
    uint32_t *inInv, *acc1, *acc2, *exp, *ptr_tmp;

    inInv = in1;

    // *********************************************************************
    // Step 1: Define the window size
    // *********************************************************************

    // tmp is set to optimal window size according to LEN

    if (state->LEN <= 2)
        tmp = 3;
    else if (state->LEN <= 7)
        tmp = 4;
    else
        tmp = 5;

    // window size is chosen such that the number 2^(w-1)+3 of required
    // registers is at most the value in aux

    for (i = 1; i <= tmp; i++) {
        if (state->aux64[0] >= ((1 << (i - 1)) + 3)) winInv = i;
    }

    // *********************************************************************
    // Step 2: Define memory mapping
    // *********************************************************************

    // accumulator
    // two registers are required because of out-of-place multiplication
    acc1 = inInv + (1 << (winInv - 1)) * OPSIZE(state->LEN);
    acc2 = acc1 + OPSIZE(state->LEN);

    // exponent
    exp = acc2 + OPSIZE(state->LEN);

    // *********************************************************************
    // Step 3: Precompute values x^3, x^5, x^7, etc.
    // *********************************************************************

    // Store x^2 at exp (temporary)
    if (winInv > 1) mMULT(exp, inInv, inInv, state->MOD, state->LEN);

    // Compute x^(2i+1) = x^(2(i-1)+1).x^2
    ptr_tmp = inInv;

    for (i = 1; i < 1 << (winInv - 1); i++) {
        mMULT(ptr_tmp + OPSIZE(state->LEN), ptr_tmp, exp, state->MOD,
            state->LEN);
        ptr_tmp += OPSIZE(state->LEN);
    }

    // *********************************************************************
    // Step 4: Compute exponent (modulus - 2)
    // *********************************************************************

    tmp = state->MOD[2];

    if (tmp == 1) {
        tmp   = 0xFFFFFFFF;
        carry = 1;
    } else {
        tmp   = tmp - 2;
        carry = 0;
    }

    // Protection to avoid writes during
    // the last Mult operation
    state->aux64[0] = tmp;
    SETWORD(state, exp);

    for (i = 1; i < state->LEN; i++) {
        tmp = state->MOD[2 + i];
        if (carry == 1) {
            if (tmp == 0) {
                tmp   = 0xFFFFFFFF;
                carry = 1;
            } else {
                tmp   = tmp - 1;
                carry = 0;
            }
        }
        exp[i] = tmp;
    }

    // *********************************************************************
    // Step 5: Exponentiation loop
    // *********************************************************************

    // Copy input in acc1

    COPY(acc1, inInv, OPSIZE(state->LEN));

    // Set i to the exponent msb -1

    // NB: non generic

    tmp = state->MOD[state->LEN + 1];
    i   = 0;

    while (tmp > 0) {
        tmp = tmp >> 1;
        i++;
    }

    i = 32 * (state->LEN - 1) + i - 2;

    while (i >= 0) {
        tmp = GETBIT(exp, (uint32_t *) ((uint32_t) i));

        if (tmp == 0) {
            // Current exponent bit is 0
            // Square accumulator
            mMULT(acc2, acc1, acc1, state->MOD, state->LEN);

            tmp  = ((uint32_t) acc1);
            acc1 = acc2;
            acc2 = ((uint32_t *) tmp);

            // Decrement loop counter
            i--;
        } else {
            // Current exponent bit is 1
            // Get current digit value (in d) and bit-length (in aux)
            state->aux64[0] = winInv;
            digit = GETDIGITL2R(state, exp, (uint32_t *) ((uint32_t) i));

            // Square accumulator aux times (aux is the digit bit-length)
            for (j = 0; j < state->aux64[0]; j++) {
                mMULT(acc2, acc1, acc1, state->MOD, state->LEN);

                tmp  = ((uint32_t) acc1);
                acc1 = acc2;
                acc2 = ((uint32_t *) tmp);
            }

            // Multipliy accumulator with precomputed value x^d
            mMULT(acc2, acc1, inInv + ((digit - 1) / 2) * OPSIZE(state->LEN),
                state->MOD, state->LEN);

            tmp  = ((uint32_t) acc1);
            acc1 = acc2;
            acc2 = ((uint32_t *) tmp);

            // Decrement loop counter aux times
            i = i - state->aux64[0];
        }
    }

    // copy result

    COPY(inInv, acc1, OPSIZE(state->LEN));

    return inInv;
}

/******************************************************************************/
/***              SQUARE AND MULTIPLY (SINGLE WORD EXPONENT)                ***/
/******************************************************************************/

// SQUAREMULTIPLYWORD primitive
// -> Perform a modular exponentiation of the element at address in in1 to the
//    power in aux (32-bit value)
// INPUT: in1 - address of the value to exponentiate
//        in2, out - available memory buffers for exponentiation
//
// OUTPUT: returned value - address of the result (either in2 or out)

uint32_t *SQUAREMULTIPLYWORD(
    ECC_State *state, uint32_t *out, uint32_t *in1, uint32_t *in2)
{
    uint32_t *acc, *dis, *ptr_tmp, f0, f1, f, t;

    dis = in2;
    acc = out;

    mMULT(out, in1, in1, state->MOD, state->LEN);

    f0 = 0;
    f1 = 0;
    f  = state->aux64[0];
    t  = (1U << 31);
    while (t != 0) {
        if (f1 != 0) {
            ptr_tmp = acc;
            mMULT(dis, acc, acc, state->MOD, state->LEN);

            acc = dis;
            dis = ptr_tmp;
        }

        if (f0 != 0) {
            f1 = 1;
            if ((f & t) != 0) {
                ptr_tmp = acc;
                mMULT(dis, acc, in1, state->MOD, state->LEN);

                acc = dis;
                dis = ptr_tmp;
            }
        }

        f0 = f0 | (f & t);
        t  = t >> 1;
    }

    return acc;
}
