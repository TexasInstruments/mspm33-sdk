/*
 * Copyright (c) 2026, Texas Instruments Incorporated
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



#ifndef I2C_HAL_COMMON
#define I2C_HAL_COMMON

#include <ti/hal/compiler/m33_ti_arm_clang/Std_Types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                 Private Definitions and Macros                             */
/* ========================================================================== */

/* ========================================================================== */
/*                 Private Typedefs                                           */
/* ========================================================================== */

/* ========================================================================== */
/*                 Private Function Prototype                                 */
/* ========================================================================== */

/* ========================================================================== */
/*                 Public Definitions and Macros                              */
/* ========================================================================== */

sint32 I2c_Hal_ControllerTransferSetup(I2c_Hal_ControllerInstanceType*          controllerInstancePtr,
                                       const I2c_Hal_ControllerTransactionType* controllerTransactionPtr,
                                       I2c_Hal_TransactionStatusType* transferStatusPtr, bool isInterruptMode);

sint32 I2c_Hal_TargetTransferSetup(I2c_Hal_TargetInstanceType*          targetInstancePtr,
                                   const I2c_Hal_TargetTransactionType* targetTransactionPtr,
                                   I2c_Hal_TransactionStatusType* transferStatusPtr, bool isInterruptMode);
#ifdef __cplusplus
}
#endif

#endif /* I2C_HAL_COMMON */
