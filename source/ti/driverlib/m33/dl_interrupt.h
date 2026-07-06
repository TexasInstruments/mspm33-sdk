/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
/*!****************************************************************************
 *  @file       dl_interrupt.h
 *  @brief      Interrupt Management Driver Library
*  @defgroup   INTERRUPT M0P Interrupts
 *
 *  @anchor ti_dl_m0p_dl_interrupt_Overview
 *  # Overview
 *
 *  The Cortex-M0+ architecture is limited to 32 device interrupts. To enable
 *  interrupt handling on devices that need more than 32 interrupts, the
 *  interrupts are grouped together.
 *
 *  For example, "Group 0" contains the interrupt flags for the WWDT0, WWDT1,
 *  DEBUGSS, FLASH, and SYSCTL peripherals. Once the interrupt fires for Group 0,
 *  the application must check to see which peripheral from Group 0 was the
 *  source using the APIs in this module.
 *
 *  This PI module is not for controlling the NVIC or the I-bit in the CPSR
 *  register. It's best to use the CMSIS-Core APIs that are delivered as part
 *  of CMSIS-Core in the `source/third_party/CMSIS/Core/Include/core_cm0plus.h`
 *  file within the SDK.
 *
 *  <hr>
 ******************************************************************************
 */
/** @addtogroup INTERRUPT
 * @{
 */
#ifndef ti_dl_m33_dl_interrupt__include
#define ti_dl_m33_dl_interrupt__include

#include <stdint.h>

#include <ti/devices/msp/msp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief      Device default Handler
 *
 *  This function is defined as part of the device startup file.
 */
extern void Default_Handler(void);


/**
 *  @brief     Register a function to be called when an interrupt occurs
 *
 *  This allows you to register a function to be called during an interrupt,
 *  allowing you to override the interrupt handler at run time. The function
 *  moves the interrupt vector table from Flash to SRAM so that the interrupt
 *  handler functions can be overwritten.
 *
 *  This function should only be used if the application requires specifying
 *  the interrupt handler at run time. If it does not need to be changed at
 *  run time consider placing the ISR function pointer in the startup file to
 *  link the function into the interrupt table.
 *
 *  Example of usage:
 *  @code
 *  DL_Interrupt_registerInterrupt(TIMA0_INT_IRQn, redirectedInterruptHandler);
*   @endcode
 *
 *  @param[in] exceptionNumber  The interrupt number to register the handler.
 *                              Interrupt numbers are device specific, please
 *                              consult the device datasheet for valid
 *                              interrupt numbers.
 *  @param[in] intHandler       A pointer to a function to call for the desired
 *                              interrupt
 */
void DL_Interrupt_registerInterrupt(
    uint32_t exceptionNumber, void (*intHandler)(void));

/**
 *  @brief      Unregister a function to be called when an interrupt occurs
 *
 *  Overrides the registered interrupt function to jump into a simple while(1)
 *  loop rather than the previously registered interrupt function. Requires
 *  that @ref DL_Interrupt_registerInterrupt was previously called so that the
 *  interrupt vector table has been moved to RAM.
 *
 *  Example of usage:
 *  @code DL_Interrupt_unregisterInterrupt(TIMA0_INT_IRQn); @endcode
 *
 *  @param[in]  exceptionNumber  The interrupt number to unregister.
 *                               Interrupt numbers are device specific, please
 *                               consult the device datasheet for valid
 *                               interrupt numbers.
 */
void DL_Interrupt_unregisterInterrupt(uint32_t exceptionNumber);

#ifdef __cplusplus
}
#endif

#endif /* ti_dl_m0p_dl_interrupt__include */
/** @}*/
