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
 * \defgroup M33_TI_ARM_CLANG_COMPILER_TYPES Compiler Specific Language Extension Header
 * \ingroup M33_TI_ARM_CLANG_COMPILER
 *
 * @{
 */

#ifndef COMPILER_H
#define COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/hal/compiler/m33_ti_arm_clang/Compiler_Cfg.h>

/* ========================================================================== */
/*                 Public Definitions and Macros                              */
/* ========================================================================== */

/** \brief Compiler version information */
#define COMPILER_MAJOR_VERSION __TI_COMPILER_MAJOR_VERSION__
/** \brief Compiler version information */
#define COMPILER_MINOR_VERSION __TI_COMPILER_MINOR_VERSION__
/** \brief Compiler version information */
#define COMPILER_PATCH_VERSION __TI_COMPILER_PATCH_VERSION__

/** \brief Used for the declaration of local pointers */
#define AUTOMATIC

/** \brief The memory class shall be used within type definitions */
#define TYPEDEF typedef

/** \brief Void pointer to zero definition */
#define NULL_PTR ((void *) 0x0)

/** \brief Abstraction of the keyword inline */
#define INLINE __inline

/** \brief Abstraction of keyword inline with static scope. */
#define LOCAL_INLINE static __inline

/** \brief Abstraction of keyword weak for weak symbols */
#ifndef __WEAK
#define __WEAK __attribute__((weak))
#endif

/**
 * \brief FUNC identification information
 *
 * The compiler abstraction shall define the FUNC macro for the declaration and definition of
 * functions that ensures correct syntax of function declarations as required by a specific
 * compiler.
 *
 * \param[in] rettype   return type of the function
 * \param[in] memclass  classification of the function itself
 */
#define FUNC(rettype, memclass) rettype

/**
 * \brief FUNC_P2CONST identification information
 *
 * The compiler abstraction shall define the FUNC_P2CONST macro for the declaration and definition
 * of functions returning a pointer to a constant. This shall ensure the correct syntax of function
 * declarations as required by a specific compiler.
 *
 * \param[in] rettype   return type of the function
 * \param[in] ptrclass  defines the classification of the pointer’s distance
 * \param[in] memclass  classification of the function itself
 *
 */
#define FUNC_P2CONST(rettype, ptrclass, memclass) const ptrclass rettype *

/**
 * \brief FUNC_P2VAR identification information
 *
 * The compiler abstraction shall define the FUNC_P2VAR macro for the declaration and definition of
 * functions returning a pointer to a variable. This shall ensure the correct syntax of function
 * declarations as required by a specific compiler.
 *
 * \param[in] rettype   return type of the function
 * \param[in] ptrclass  defines the classification of the pointer’s distance
 * \param[in] memclass  classification of the function itself
 *
 */
#define FUNC_P2VAR(rettype, ptrclass, memclass) ptrclass rettype *

/**
 * \brief P2VAR identification information
 *
 * The compiler abstraction shall define the P2VAR macro for the declaration and definition of
 * pointers in RAM, pointing to variables. The pointer itself is modifiable (e.g. ExamplePtr++). The
 * pointer’s target is modifiable (e.g. *ExamplePtr = 5).
 *
 * \param[in] ptrtype   type of the referenced variable
 * \param[in] memclass  classification of the variable itself
 * \param[in] ptrclass  defines the classification of the pointer’s distance
 *
 */
#define P2VAR(ptrtype, memclass, ptrclass) ptrtype *

/**
 * \brief P2CONST identification information
 *
 * The compiler abstraction shall define the P2CONST macro for the declaration and definition of
 * pointers in RAM pointing to constants. The pointer itself is modifiable (e.g. ExamplePtr++). The
 * pointer’s target is not modifiable (read only).
 *
 * \param[in] ptrtype   type of the referenced variable
 * \param[in] memclass  classification of the variable itself
 * \param[in] ptrclass  defines the classification of the pointer’s distance
 *
 */
#define P2CONST(ptrtype, memclass, ptrclass) const ptrtype *

/**
 * \brief CONSTP2VAR identification information
 *
 * The compiler abstraction shall define the CONSTP2VAR macro for the declaration and definition of
 * constant pointers accessing variables. The pointer itself is not modifiable (fix address). The
 * pointer’s target is modifiable (e.g. *ExamplePtr = 18).
 *
 * \param[in] ptrtype   type of the referenced variable
 * \param[in] memclass  classification of the variable itself
 * \param[in] ptrclass  defines the classification of the pointer’s distance
 *
 */
#define CONSTP2VAR(ptrtype, memclass, ptrclass) ptrtype *const

/**
 * \brief CONSTP2CONST identification information
 *
 * The compiler abstraction shall define the CONSTP2CONST macro for the declaration and definition
 * of constant pointers accessing constants. The pointer itself is not modifiable (fix address). The
 * pointer’s target is not modifiable (read only).
 *
 * \param[in] ptrtype   type of the referenced variable
 * \param[in] memclass  classification of the variable itself
 * \param[in] ptrclass  defines the classification of the pointer’s distance
 *
 */
#define CONSTP2CONST(ptrtype, memclass, ptrclass) const ptrtype *const

/**
 * \brief P2FUNC identification information
 *
 * The compiler abstraction shall define the P2FUNC macro for the type definition of pointers to
 * functions.
 *
 * \param[in] rettype   return type of the function
 * \param[in] ptrclass  defines the classification of the pointer’s distance
 * \param[in] fctname   function name respectively name of the defined type
 *
 */
#define P2FUNC(rettype, ptrclass, fctname) rettype(*fctname)

/**
 * \brief CONSTP2FUNC identification information
 *
 * The compiler abstraction shall define the CONSTP2FUNC macro for the type definition of constant
 * pointers to functions.
 *
 * \param[in] rettype   return type of the function
 * \param[in] ptrclass  defines the classification of the pointer’s distance
 * \param[in] fctname   function name respectively name of the defined type
 *
 */
#define CONSTP2FUNC(rettype, ptrclass, fctname) rettype(*const fctname)

/**
 * \brief CONST identification information
 *
 * The compiler abstraction shall define the CONST macro for the declaration and definition of
 * constants.
 *
 * \param[in] consttype return type of the function
 * \param[in] memclass  defines the classification of the const variable's distance
 *
 */
#define CONST(consttype, memclass) const consttype

/**
 * \brief VAR identification information
 *
 * The compiler abstraction shall define the VAR macro for the declaration and definition of
 * variables.
 *
 * \param[in] vartype   return type of the function
 * \param[in] memclass  defines the classification of the variable's distance
 *
 */
#define VAR(vartype, memclass) vartype

/* ========================================================================== */
/*                 Public Typedefs                                            */
/* ========================================================================== */

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* COMPILER_H */

/**@}*/
