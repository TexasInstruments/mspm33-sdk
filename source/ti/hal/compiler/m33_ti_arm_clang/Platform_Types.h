/*
 *  Copyright (C) 2025 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \defgroup M33_TI_ARM_CLANG_PLATFORM_TYPES Platform Types
 * \ingroup M33_TI_ARM_CLANG_COMPILER
 *
 * @{
 */

#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                 Public Definitions and Macros                              */
/* ========================================================================== */

/** \brief Indicating a 8 bit processor */
#define CPU_TYPE_8  8
/** \brief Indicating a 16 bit processor */
#define CPU_TYPE_16 16
/** \brief Indicating a 32 bit processor */
#define CPU_TYPE_32 32
/** \brief Indicating a 64 bit processor */
#define CPU_TYPE_64 64

/** \brief The most significant bit is the first bit of the bit sequence. */
#define MSB_FIRST 0
/** \brief The least significant bit is the first bit of the bit sequence. */
#define LSB_FIRST 1

/** \brief Within uint16, the high byte is located before the low byte. */
#define HIGH_BYTE_FIRST 0
/** \brief Within uint16, the low byte is located before the high byte. */
#define LOW_BYTE_FIRST  1

#ifndef TRUE
/** \brief Represents boolean true value */
#define TRUE ((boolean)1)
#endif

#ifndef FALSE
/** \brief Represents boolean false value */
#define FALSE ((boolean)0)
#endif

/** \brief Register width of the CPU used */
#define CPU_TYPE CPU_TYPE_32

/** \brief Bit order on register level */
#define CPU_BIT_ORDER LSB_FIRST

#if !defined(__ARM_BIG_ENDIAN)
/** \brief Byte order on memory level */
#define CPU_BYTE_ORDER LOW_BYTE_FIRST
#else
/** \brief Byte order on memory level */
#define CPU_BYTE_ORDER HIGH_BYTE_FIRST
#endif

/* ========================================================================== */
/*                 Public Typedefs                                            */
/* ========================================================================== */

#if !defined(__TI_COMPILER_VERSION__)

/** \brief This standard type shall only be used together with the definitions TRUE and FALSE. */
typedef unsigned char boolean;

/** \brief This standard type shall be of 8 bit unsigned. */
typedef unsigned char      uint8;
/** \brief This standard type shall be of 16 bit unsigned. */
typedef unsigned short     uint16;
/** \brief This standard type shall be of 32 bit unsigned. */
typedef unsigned int       uint32;
/** \brief This standard type shall be of 64 bit unsigned. */
typedef unsigned long long uint64;

/** \brief This standard type shall be of 8 bit signed. */
typedef signed char sint8;
/** \brief This standard type shall be of 16 bit signed. */
typedef short       sint16;
/** \brief This standard type shall be of 32 bit signed. */
typedef int         sint32;
/** \brief This standard type shall be of 64 bit signed. */
typedef long long   sint64;

/** \brief This optimized type shall be at least 8 bit unsigned. */
typedef unsigned int uint8_least;
/** \brief This optimized type shall be at least 16 bit unsigned. */
typedef unsigned int uint16_least;
/** \brief This optimized type shall be at least 32 bit unsigned. */
typedef unsigned int uint32_least;

/** \brief This optimized type shall be at least 8 bit signed. */
typedef signed int sint8_least;
/** \brief This optimized type shall be at least 16 bit signed. */
typedef signed int sint16_least;
/** \brief This optimized type shall be at least 32 bit signed. */
typedef signed int sint32_least;

/**
 * \brief This standard type shall follow the 32-bit binary interchange format according to IEEE
 * 754-2008 with encoding parameters specified in chapter 3.6, table 3.5, column "binary32".
 */
typedef float  float32;
/** \brief This standard type shall follow the 32-bit binary interchange format according to IEEE
 * 754-2008 with encoding parameters specified in chapter 3.6, table 3.5, column "binary64".
 */
typedef double float64;

#endif

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_TYPES_H */

/**@}*/
