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
 * \defgroup M33_TI_ARM_CLANG_STD_TYPES Standard Types
 * \ingroup  M33_TI_ARM_CLANG_COMPILER
 *
 * @{
 */

#ifndef STD_TYPES_H
#define STD_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/hal/compiler/m33_ti_arm_clang/Compiler.h>
#include <ti/hal/compiler/m33_ti_arm_clang/Platform_Types.h>

/* ========================================================================== */
/*                 Public Definitions and Macros                              */
/* ========================================================================== */

#ifndef STATUSTYPEDEFINED
/** \brief Status type defined */
#define STATUSTYPEDEFINED
/**
 * \brief Return OK
 *
 * \mcal_only_type
 */
#define E_OK ((uint8)0x00U)
/**
 * \brief Status type
 *
 * \mcal_only_type
 */
typedef unsigned char StatusType;
#endif
/**
 * \brief Return Not OK
 *
 * \mcal_only_type
 */
#define E_NOT_OK ((uint8)0x01U)

/** \brief Physical state 5V or 3.3V */
#define STD_HIGH 0x01U
/** \brief Physical state 0v         */
#define STD_LOW  0x00U

/** \brief Logical state active */
#define STD_ACTIVE 0x01U
/** \brief Logical state idle   */
#define STD_IDLE   0x00U

/** \brief Turn ON */
#define STD_ON  0x01U
/** \brief Turn OFF */
#define STD_OFF 0x00U

/* ========================================================================== */
/*                 Public Typedefs                                            */
/* ========================================================================== */

/**
 * \brief Return type
 *
 * \mcal_only_type
 *
 * This type can be used as standard API return type which is shared
 * between the RTE and the BSW modules.
 *
 */
typedef uint8 Std_ReturnType;

/**
 * \brief Version Info
 *
 * \mcal_only_type
 *
 * This type shall be used to request the version of a BSW module using
 * the `<Module name>_GetVersionInfo()` function.
 */
typedef struct Std_VersionInfoTag
{
    /** \brief Vendor ID */
    uint16 vendorID;
    /** \brief Module ID */
    uint16 moduleID;
    /** \brief Instance */
    uint8  instanceID;
    /** \brief Software Major version */
    uint8  sw_major_version;
    /** \brief Software Minor version */
    uint8  sw_minor_version;
    /** \brief Software Patch version */
    uint8  sw_patch_version;
} Std_VersionInfoType;

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif  // STD_TYPES_H

/** @} */

/**
 * \defgroup M33_TI_ARM_CLANG_COMPILER  M33 TI ARM CLANG compiler specific types
 * \ingroup COMPILER
 */
