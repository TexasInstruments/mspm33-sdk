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

/*
 *  Description:  It contains module specific parameters (ptrclass and memclass) that are passed to
 *                the macros defined in Compiler.h
 */

#ifndef COMPILER_CFG_H
#define COMPILER_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This is an empty header file which should be replaced by the end user.
 *
 * According to the AUTOSAR document "Specification of Compiler Abstraction"
 * final version, as part of AUTOSAR release 4.2.2, SWS_COMPILER_00055:
 *  The file Compiler_Cfg.h shall contain the module specific parameters
 *  (ptrclass and memclass) that are passed to the macros defined in
 *  Compiler.h.
 *
 * The following is the form of these macro definitions.  MSN stands for
 * "Module Short Name of BSW module list"
 *
 * #define <MSN>_CODE
 * #define <MSN>_CONST
 * #define <MSN>_APPL_DATA
 * #define <MSN>_APPL_CONST
 * #define <MSN>_APPL_CODE
 * #define <MSN>_VAR_NOINIT
 * #define <MSN>_VAR_POWER_ON_INIT
 * #define <MSN>_VAR_FAST
 * #define <MSN>_VAR
 *
 * NOTE: On 32 bit architectures these definitions are required to be empty
 */

/* ========================================================================== */
/*                 Public Definitions and Macros                              */
/* ========================================================================== */

/* ========================================================================== */
/*                 Public Typedefs                                            */
/* ========================================================================== */

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* COMPILER_CFG_H */
