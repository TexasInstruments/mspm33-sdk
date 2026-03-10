/********************************************************************************
 **                                                                            **
 **                      Copyright 2006 -- 2012 (c)  INVIA                     **
 **                                                                            **
 **   All rights reserved. Reproduction in whole or part is prohibited         **
 **      without the written permission of the copyright owner                 **
 **                                                                            **
 ********************************************************************************
 ** File Name:      lowlevelapi.c
 ** Release:        2.0
 ** Author:         Alexandre Berzati <alexandre.berzati@invia.fr> - INVIA
 **
 ** Description:    Low-level arithmetic routines that simulate the MEXPA
 ** Note:           This source file provides MEXPA implementation of most
 **                 of arithmetic functions used to run elliptic curves.
 **                 Refer to the User Manual for further details
 **
 ********************************************************************************
 */

/*
 * Customizations:
 *  - Removed global variables and modified APIs accordingly.
 *  - Modified to use stdint.
 *  - Added BUFFERSEQUAL().
 *  - Added ISALLZEROS().
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

#include "lowlevelapi.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

//-------------------------------------------------------------------------------
// Global variables made available outside of this module
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Global variables that remain within the scope of this module
//-------------------------------------------------------------------------------

//(((((((((((((((((((((((((((((((((((((((IMPORTLENGTH))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Returns the wordsize of the integer given at address in1 in LV format
//
//  Interface
//
//      Arguments are given through global variables:
//
//      in1     points to  integer in LV format [L, LSW, ..., MSW]
//      [ret]   length of the array
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
uint32_t IMPORTLENGTH(const uint32_t *in1)
{
    return in1[0];
}

//((((((((((((((((((((((((((((((((((((((IMPORTMODULUS))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Imports the integer in LV format found at address in1 into address out in modulus
//      format [0, 0, LSW, ..., MSW]. Returns the wordsize of the imported modulus.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      ..
//
//      Operands:
//
//      in1     points to  integer in LV format [L, LSW, ..., MSW]
//      out     points where the modulus is to be written
//      [ret]   length of the modulus in 32-bit words
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
uint32_t IMPORTMODULUS(uint32_t *out, const uint32_t *in1)
{
    uint32_t L, i;

    L = in1[0];

    out[0] = 0;
    out[1] = 0;

    for (i = 1; i < L + 1; i++) out[i + 1] = in1[i];

    return L;
}

//((((((((((((((((((((((((((((((((((((((IMPORTOPERAND))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Imports the integer in LV format found at address in1 into address out in operand
//      format [LSW, ..., MSW, 0, ..., 0], making sure that the obtained array has LEN+1
//      words, thus truncating or right-padding with zeroes as needed. Returns the wordsize
//      of the imported integer.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      in1     points to  integer in LV format [L, LSW, ..., MSW]
//      out     points where the operand is to be written
//      [ret]   Length of the imported operand in 32-bit words,
//              which is either LEN or LEN + 1, if the operand is longer than LEN
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
uint32_t IMPORTOPERAND(ECC_State *state, uint32_t *out, const uint32_t *in1)
{
    uint32_t L, i;

    L = in1[0];

    if (L > state->LEN + 1) L = state->LEN + 1;

    for (i = 0; i < L; i++) out[i] = in1[i + 1];
    for (i = L; i < OPSIZE(state->LEN); i++) out[i] = 0;

    return L;
}

//((((((((((((((((((((((((((((((((((((((EXPORTOPERAND))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Exports the integer found at address in1 to address out in LV
//      format [LEN, LSW, ..., MSW]. Returns LEN.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      in1     points to  integer
//      out     points where the integer is to be written
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
uint32_t EXPORTOPERAND(ECC_State *state, uint32_t *out, const uint32_t *in1)
{
    uint32_t i;

    out[0] = state->LEN;
    for (i = 0; i < state->LEN; i++) out[i + 1] = in1[i];

    return state->LEN;
}

//(((((((((((((((((((((((((((((((((((((((IMPORTDATA))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Imports the integer in LV format found at address in1 into address out in raw data
//      format [LSW, ..., MSW]. Returns the wordsize of the imported integer.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      ..
//
//      Operands:
//
//      in1     points to  integer in LV format [L, LSW, ..., MSW]
//      out     points where the integer is to be written
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
uint32_t IMPORTDATA(uint32_t *out, const uint32_t *in1)
{
    uint32_t L, i;

    L = in1[0];

    for (i = 0; i < L; i++) out[i] = in1[i + 1];

    return L;
}

//((((((((((((((((((((((((((((((((((((((SETOPERAND))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Writes the 32-bit integer in aux at address out in operand format
//
//  Interface
//
//      Arguments are given through global variables:
//
//      LEN --> length of m in 32-bit words
//
//      Operands:
//
//      inValue   word to copy
//      out       points where the integer is to be written
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
void SETOPERAND(ECC_State *state, uint32_t *out, uint32_t inValue)
{
    uint32_t i;

    out[0] = inValue;
    for (i = 1; i < OPSIZE(state->LEN); i++) out[i] = 0;
}

//(((((((((((((((((((((((((((((((((((((((((((COPY)))))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Copies aux words from address in1 to address out.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      ..
//
//      Operands:
//
//      in1     points anywhere in memory
//      out     points anywhere in memory
//      size    number of words to be copied
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
void COPY(uint32_t *out, uint32_t *in1, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < size; i++) out[i] = in1[i];
}

//(((((((((((((((((((((((((((((((((((((((((((ZERO)))))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Writes zeroes at addresses out through out + aux - 1.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      ..
//
//      Operands:
//
//      out     points anywhere in memory
//      size    number of zeroes to be written
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
void ZERO(uint32_t *out, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < size; i++) out[i] = 0;
}

//((((((((((((((((((((((((((((((((((((((SETWORD))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Writes the 32-bit integer in aux at address out + WordPos
//
//  Interface
//
//      Arguments are given through global variables:
//
//      ..
//
//      Operands:
//
//      inValue   word to copy
//      out       points where the integer is to be written
//
//  Developer's notes
//
//-------------------------------------------------------------------------------

void SETWORD(ECC_State *state, uint32_t *out)
{
    out[0] = state->aux64[0];
}

//-------------------------------------------------------------------------------
// Initializing modular computations
//-------------------------------------------------------------------------------

//(((((((((((((((((((((((((((((((((((((((((((mSET)))))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Initializes the modulus at address in1 for later modular operations. Completes the
//      modulus with the modulus-specific constant alpha = -1/m mod 2^64.
//
//  Interface
//
//      Operands:
//
//      state   points to ECC state object
//      in1     points to modulus in format [0, 0, LSW(m), ..., MSW(m)]
//      size    length of the modulus in 32-bit words
//
//  Developer's notes
//
//      In simulation mode, only al0 = alpha mod 2^32 is computed. The routine uses the
//      Newton-based inductive formula alpha = alpha * (m[0] * alpha + 2) mod 2^32
//      repeated 6 times where alpha is previously set to 1.
//
//-------------------------------------------------------------------------------
void mSET(ECC_State *state, uint32_t *in1, uint32_t size)
{
    uint32_t i;
    uint64_t C;

    uint64_t A, M;

    M = (((uint64_t) in1[3]) << 32) + in1[2];
    A = M + 2;
    A = A * (A * M + 2);
    A = A * (A * M + 2);
    A = A * (A * M + 2);
    A = A * (A * M + 2);
    A = A * (A * M + 2);

    in1[0] = (uint32_t) A;
    in1[1] = A >> 32;

    state->MSBMOD = in1[size + 1] >> 31;

    if (state->MSBMOD) {
        C = 1;
        for (i = 0; i < size; i++) {
            in1[size + 2 + i] = ~in1[2 + i] + C;
            if (C && (in1[size + 2 + i] == 0))
                C = 1;
            else
                C = 0;
        }
    } else {
        C = 2;
        for (i = 0; i < size; i++) {
            C += ((uint64_t) ~in1[2 + i]) << 1;
            in1[size + 2 + i] = (uint32_t) C;
            C                 = C >> 32;
        }
    }

    state->MOD = in1;
    state->LEN = size;
}

//(((((((((((((((((((((((((((((((((((((((((zSET))))))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Prepares operation in Integer
//
//  Interface
//
//      Arguments are given through global variables:
//
//      ..
//
//      Operands:
//
//      size    indicates the wordsize of the operands
//
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
void zSET(ECC_State *state, uint32_t size)
{
    state->LEN = size;
}

//((((((((((((((((((((((((((((((((((((((((((mINVK))))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Writes c = 1/K mod MOD at on LEN words at address out where K is the Montgomery
//      factor K = 2^-(32.LEN) mod MOD.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      A modulus must have been initialized by the mSET routine.
//      MOD --> [LSW(alpha), MSW(alpha), LSW(m), ..., MSW(m)]
//      LEN --> length of m in 32-bit words
//
//      Operands:
//
//      out     points anywhere in memory
//
//
//  Developer's notes
//
//      ...
//
//-------------------------------------------------------------------------------
void mINVK(ECC_State *state, uint32_t *out)
{
    volatile int i, j;
    uint64_t n;
    uint32_t d;

    // Prepare the modulus
    for (i = 2; i < (state->LEN + 2); i++) state->MOD[i] ^= 0xFFFFFFFF;

    SETOPERAND(state, out, 1);
    zADD(state, out, out, &state->MOD[2]);

    // Perform an extra reduction when the value of the output
    // Exceed the module. This may happen when the MSW of the modulus
    // is smaller than 0x10000000
    // Compute d on a full word
    n = out[state->LEN];
    n = n << 32 | out[state->LEN - 1];

    d = (uint32_t)((state->MOD[state->LEN + 1] ^ 0xFFFFFFFF) + 1);

    // Avoid division by zero
    if (d == 0)
        d = (uint32_t)(n >> 32);
    else
        d = n / d;

    // Safely reduce normalization
    // Factor to avoid extra reduction
    state->aux64[0] = d;
    zADD32(state, out, out);

    zMACC32(state, out, &state->MOD[2], out);
    out[state->LEN] = 0;

    // Then perform a regular Loop to deterùine the Montgomery
    // Constant
    for (i = 0; i < 2 * ((MONTLOG(state->LEN) >> 5) - state->LEN); i++) {
        // Shift by 2^16 => reduce
        for (j = state->LEN; j > 0; j--)
            out[j] = (out[j] << 16) | (out[j - 1] >> 16);
        out[0] <<= 16;

        // Compute d on a full word
        n = out[state->LEN];
        n = n << 32 | out[state->LEN - 1];

        d = (uint32_t)((state->MOD[state->LEN + 1] ^ 0xFFFFFFFF) + 1);

        // Avoid division by zero
        if (d == 0)
            d = (uint32_t)(n >> 32);
        else
            d = n / d;

        // Safely reduce normalization
        // Factor to avoid extra reduction
        state->aux64[0] = d;
        zADD32(state, out, out);

        zMACC32(state, out, &state->MOD[2], out);
        out[state->LEN] = 0;

        // MSW(out) = 0 || *
        // out = out - d * MOD
    }

    // Compute Back the modulus
    for (i = 2; i < (state->LEN + 2); i++) state->MOD[i] ^= 0xFFFFFFFF;
}

//-------------------------------------------------------------------------------
// Integer operators
//-------------------------------------------------------------------------------

//(((((((((((((((((((((((((((((((((((((((((((zADD)))))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Computes out = in1 + in2 on LEN words and returns the overflow carry.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      LEN --> length of operands in 32-bit words
//
//      Operands:
//
//      in1     points to first operand
//      in2     points to second operand
//      out     points to result to be written
//
//  Developer's notes
//
//-------------------------------------------------------------------------------

uint32_t zADD(ECC_State *state, uint32_t *out, uint32_t *in1, uint32_t *in2)
{
    uint16_t A1, A0, B1, B0, C, i;
    uint32_t t;

    C = 0;
    for (i = 0; i < state->LEN; i++) {
        t  = in1[i];
        A1 = (uint16_t)(t >> 16);
        A0 = (uint16_t) t;

        t  = in2[i];
        B1 = (uint16_t)(t >> 16);
        B0 = (uint16_t) t;

        t  = (uint32_t)(B0 + A0 + C);
        C  = (uint16_t)(t >> 16);
        B0 = (uint16_t) t;

        t  = (uint32_t)(B1 + A1 + C);
        C  = (uint16_t)(t >> 16);
        B1 = (uint16_t) t;

        out[i] = B1;
        out[i] <<= 16;
        out[i] |= B0;
    }
    return (uint32_t) C;
}

//(((((((((((((((((((((((((((((((((((((((((((zSUB)))))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Computes out = in1 - in2 on LEN words and returns the Borrow.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      LEN --> length of operands in 32-bit words
//
//      Operands:
//
//      in1     points to  first operand
//      in2     points to  second operand
//      out     points to  result to be written
//
//  Developer's notes
//
//-------------------------------------------------------------------------------

uint32_t zSUB(ECC_State *state, uint32_t *out, uint32_t *in1, uint32_t *in2)
{
    uint16_t A1, A0, B1, B0, C, i;
    uint32_t t;

    // To emulate - in2
    C  = 1;
    t  = in1[0];
    A1 = (uint16_t)(t >> 16);
    A0 = (uint16_t) t;

    t  = in2[0] ^ 0xFFFFFFFF;
    B1 = (uint16_t)(t >> 16);
    B0 = (uint16_t) t;

    t  = (uint32_t)(B0 + A0 + C);
    C  = (uint16_t)(t >> 16);
    B0 = (uint16_t) t;

    t  = (uint32_t)(B1 + A1 + C);
    C  = (uint16_t)(t >> 16);
    B1 = (uint16_t) t;

    out[0] = (uint32_t)(B1 << 16) | B0;

    for (i = 1; i < state->LEN + 1; i++) {
        t  = in1[i];
        A1 = (uint16_t)(t >> 16);
        A0 = (uint16_t) t;

        t  = in2[i] ^ 0xFFFFFFFF;
        B1 = (uint16_t)(t >> 16);
        B0 = (uint16_t) t;

        t  = (uint32_t)(B0 + A0 + C);
        C  = (uint16_t)(t >> 16);
        B0 = (uint16_t) t;

        t  = (uint32_t)(B1 + A1 + C);
        C  = (uint16_t)(t >> 16);
        B1 = (uint16_t) t;

        out[i] = (uint32_t)(B1 << 16) | B0;
    }
    // test borrow
    if (out[state->LEN] == 0xFFFFFFFF) {
        out[state->LEN] = 0;
        return 1;
    }
    // Otherwise
    return 0;
}

//(((((((((((((((((((((((((((((((((((((((((zMACC32)))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Computes out = (in1 mod 2^(32 LEN)) * aux + in2 mod 2^(32 LEN)) and returns the
//      overflow carry.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      LEN --> length of operands in 32-bit words
//      aux --> 32-bit integer (representing value to multiply in1 by)
//
//      Operands:
//
//      in1     points to first operand
//      in2     points to second operand
//      out     points to result to be written
//
//  Developer's notes
//
//-------------------------------------------------------------------------------

void zMACC32(ECC_State *state, uint32_t *out, uint32_t *in1, uint32_t *in2)
{
    uint16_t aux0, aux1, A0, A1, B0, B1, D, C, s0, s1;
    uint32_t t, i;

    aux0 = (uint16_t) state->aux64[0];
    aux1 = (uint16_t)(state->aux64[0] >> 16);

    D = 0;
    C = 0;
    for (i = 0; i < state->LEN; i++) {
        t  = in1[i];
        A1 = (uint16_t)(t >> 16);
        A0 = (uint16_t) t;

        t  = in2[i];
        B1 = (uint16_t)(t >> 16);
        B0 = (uint16_t) t;

        t  = (uint32_t)(A0 * aux0 + C + B0);
        s1 = (uint16_t)(t >> 16);
        s0 = (uint16_t) t;

        t  = (uint32_t)(A1 * aux0 + D + s1);
        C  = (uint16_t)(t >> 16);
        s1 = (uint16_t) t;

        t  = (uint32_t)(A0 * aux1 + B1 + s1);
        D  = (uint16_t)(t >> 16);
        s1 = (uint16_t) t;

        t = (uint32_t)(A1 * aux1 + C + D);
        D = (uint16_t)(t >> 16);
        C = (uint16_t) t;

        out[i] = (uint32_t)(s1 << 16) | s0;
    }

    t = (uint32_t) C;

    s1 = (uint16_t)(t >> 16);
    s0 = (uint16_t) t;

    t  = (uint32_t)(D + s1);
    s1 = (uint16_t) t;
    t  = (uint32_t)(t >> 16);

    out[i] = (uint32_t)(s1 << 16) | s0;
}

// (((((((((((((((((((((((((((((((((((((((((zSUB32)))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Computes @(out) = @(in1) - aux on LEN words and returns the overflow carry.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      LEN --> length of operands in 32-bit words
//      aux --> 32-bit integer (representing value to subtract)
//
//      Operands:
//
//      in1     points to first operand
//      out     points to result to be written
//
//  Developer's notes
//
//-----------------------------------------------------------------------------------------
uint32_t zSUB32(ECC_State *state, uint32_t *out, uint32_t *in1)
{
    uint8_t i;
    uint64_t A;
    uint32_t C;

    C = 1;
    A = in1[0];
    A = A + ((uint64_t)(~state->aux64[0])) + ((uint64_t) C);

    out[0] = (uint32_t) A;
    C      = (uint32_t)(A >> 32);

    for (i = 1; i < state->LEN; i++) {
        A      = ((uint64_t) in1[i]) + 0xFFFFFFFF + ((uint64_t) C);
        out[i] = (uint32_t) A;
        C      = (uint32_t)(A >> 32);
    }

    return ~C;
}

// (((((((((((((((((((((((((((((((((((((((((zADD32)))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Computes @(out) = @(in1) + aux on LEN words and returns the overflow carry.
//
//  Interface
//
//      Arguments are given through global variables:
//
//      LEN --> length of operands in 32-bit words
//      aux --> 32-bit integer (representing value to subtract)
//
//      Operands:
//
//      in1     points to first operand
//      out     points to result to be written
//
//  Developer's notes
//
//-----------------------------------------------------------------------------------------
uint32_t zADD32(ECC_State *state, uint32_t *out, uint32_t *in1)
{
    uint8_t i;
    uint16_t A1, A0;
    uint32_t t, C;

    C  = 0;
    t  = in1[0];
    A1 = (uint16_t)(t >> 16);
    A0 = (uint16_t) t;

    t  = (uint32_t)(((uint16_t) state->aux64[0]) + A0 + C);
    C  = (uint16_t)(t >> 16);
    A0 = (uint16_t) t;

    t  = (uint32_t)(((uint16_t)(state->aux64[0] >> 16)) + A1 + C);
    C  = (uint16_t)(t >> 16);
    A1 = (uint16_t) t;

    out[0] = (uint32_t)(A1 << 16) | A0;

    for (i = 1; i < state->LEN; i++) {
        t  = in1[i];
        A1 = (uint16_t)(t >> 16);
        A0 = (uint16_t) t;

        t  = (uint32_t)(A0 + C);
        C  = (uint16_t)(t >> 16);
        A0 = (uint16_t) t;

        t  = (uint32_t)(A1 + C);
        C  = (uint16_t)(t >> 16);
        A1 = (uint16_t) t;

        out[i] = (uint32_t)(A1 << 16) | A0;
    }
    return C;
}

//(((((((((((((((((((((((((((((((((((((((((zMULT32)))))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Computes out = (in1 mod 2^(32 LEN)) * aux mod 2^(32(LEN+1)).
//
//  Interface
//
//      Arguments are given through global variables:
//
//      LEN --> length of operands in 32-bit words
//      aux --> 32-bit integer (representing value to multiply)
//
//      Operands:
//
//      in1     points to first operand
//      out     points to result to be written
//
//  Developer's notes
//
//-------------------------------------------------------------------------------

void zMULT32(ECC_State *state, uint32_t *out, uint32_t *in1)
{
    uint16_t aux0, aux1, A0, A1, D, C, s0, s1;
    uint32_t t, i;

    aux0 = (uint16_t) state->aux64[0];
    aux1 = (uint16_t)(state->aux64[0] >> 16);

    D = 0;
    C = 0;
    for (i = 0; i < state->LEN; i++) {
        t  = in1[i];
        A1 = (uint16_t)(t >> 16);
        A0 = (uint16_t) t;

        t  = (uint32_t)(A0 * aux0 + C);
        s1 = (uint16_t)(t >> 16);
        s0 = (uint16_t) t;

        t  = (uint32_t)(A1 * aux0 + D + s1);
        C  = (uint16_t)(t >> 16);
        s1 = (uint16_t) t;

        t  = (uint32_t)(A0 * aux1 + s1);
        D  = (uint16_t)(t >> 16);
        s1 = (uint16_t) t;

        t = (uint32_t)(A1 * aux1 + C + D);
        D = (uint16_t)(t >> 16);
        C = (uint16_t) t;

        out[i] = (uint32_t)(s1 << 16) | s0;
    }

    out[i] = (uint32_t)(D << 16) | C;
}

//((((((((((((((((((((((((((((((((((((((BUFFERSEQUAL)))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Compares two buffers in constant time.
//
//  Interface
//
//      buffer0     points to first uint32_t buffer
//      buffer1     points to second uint32_t buffer
//      bufferWordLength    Number of 32-bit words of buffer0 and buffer1 to compare
//
//      Returns true if buffer content is identical and false if not.
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
bool BUFFERSEQUAL(const uint32_t *buffer0, const uint32_t *buffer1,
    uint32_t bufferWordLength)
{
    uint32_t tempResult = 0;
    uint32_t i;

    /* XOR each 32-bit word of the buffer together and OR the results.
     * If the OR'd result is non-zero, the buffers do not match.
     * There is no branch based on the content of the buffers here to avoid
     * timing attacks.
     */
    for (i = 0; i < bufferWordLength; i++) {
        tempResult |= buffer0[i] ^ buffer1[i];
    }

    return tempResult == 0 ? true : false;
}

//(((((((((((((((((((((((((((((((((((((((ISALLZEROS)))))))))))))))))))))))))))))))))))))))
//
//  High-level description
//
//      Checks if a buffer contains only zeros in constant time
//
//  Interface
//
//      buffer      points to the uint32_t buffer to check
//      bufferWordLength    Number of 32-bit words of buffer
//
//      Returns true if the buffer only contains 0x00000000 and false elsewise
//
//  Developer's notes
//
//-------------------------------------------------------------------------------
bool ISALLZEROS(const uint32_t *buffer, uint32_t bufferWordLength)
{
    uint32_t i;
    uint32_t bufferBits = 0;

    for (i = 0; i < bufferWordLength; i++) {
        bufferBits |= buffer[i];
    }

    if (bufferBits == 0) {
        return true;
    } else {
        return false;
    }
}
//==================================================================================== end>
