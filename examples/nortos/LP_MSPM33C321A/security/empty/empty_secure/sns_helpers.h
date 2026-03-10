/*
 * Copyright (c) 2025, Texas Instruments Incorporated - http://www.ti.com
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

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m33/dl_core.h>

#ifndef __SNS_HELPERS_H__
#define __SNS_HELPERS_H__

/**
 * @brief Non-Secure Function Call Attribute
 * @note Use this attribute for function pointers that call non-secure functions from secure code
 * @note Generates secure gateway veneers for transitions from secure to non-secure state
 */
#define NS_FUNC       __attribute__( ( cmse_nonsecure_call ) )

/**
 * @brief Non-Secure Callable Entry Attribute
 * @note Use this attribute for secure functions that can be called from non-secure code
 * @note Functions with this attribute must be placed in NSC (Non-Secure Callable) memory regions
 */
#define NS_FUNC_ENTRY __attribute__( ( cmse_nonsecure_entry ) )

/**
 * @brief Function pointer type for non-secure function calls
 * @note This type definition includes the NS_FUNC attribute for safe non-secure transitions
 * @note Use this type when declaring function pointers that will call non-secure functions
 */
typedef NS_FUNC void ( *volatile nonsecure_func_t )( void );

/**
 * @brief Function pointer type for non-secure functions with parameters
 * @param param Generic parameter for non-secure function calls
 * @note Extended function pointer type for non-secure calls with single parameter
 */
typedef NS_FUNC void ( *volatile nonsecure_func_param_t )( uint32_t param );

/**
 * @brief Function pointer type for non-secure functions with return value
 * @return uint32_t Return value from non-secure function
 * @note Function pointer type for non-secure calls that return a value
 */
typedef NS_FUNC uint32_t ( *volatile nonsecure_func_ret_t )( void );

/**
 * @brief Validate if address is in non-secure memory
 * @param addr Address to validate
 * @param size Size of memory region to check
 * @return 1 if address is non-secure, 0 if secure or invalid
 */
#define IS_NS_ADDRESS( addr, size ) cmse_check_address_range( ( void * ) ( addr ), ( size ), CMSE_NONSECURE )

/**
 * @brief Validate if function pointer is non-secure
 * @param fptr Function pointer to validate
 * @return 1 if function is non-secure, 0 if secure or invalid
 */
#define IS_NS_FUNCTION( fptr )      cmse_is_nsfptr( fptr )

#endif /* __SNS_HELPERS_H__ */
