/*****************************************************************************

  Copyright (C) 2026 Texas Instruments Incorporated - http://www.ti.com/

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

   Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

   Neither the name of Texas Instruments Incorporated nor the names of
   its contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/

/*
 * Linker command file for MSPM33C321A Application
 *
 * This linker file places the application at 0x10080000 to work with
 * the secondary_bsl_empty bootloader.
 *
 * Memory Layout (1MB total flash):
 * - Bootloader FLASH: 0x10000000 - 0x1007FFFF (512KB reserved for bootloader)
 * - Application FLASH: 0x10080000 - 0x100FFFFF (512KB for application)
 * - SRAM: 0x30000000 - 0x3003FFFF (256KB)
 */

-uinterruptVectors
--stack_size=512

MEMORY
{
    /*
     * Application FLASH region
     * Origin: 0x10080000 (512KB offset from flash start)
     * Length: 0x0007FFFF (512KB remaining flash space)
     */
    FLASH           (RX)  : origin = 0x10080000, length = 0x0007FFFF

    /*
     * SRAM region
     * Origin: 0x30000000
     * Length: 0x0003FFFF (256KB)
     */
    SRAM            (RWX) : origin = 0x30000000, length = 0x0003FFFF

    /*
     * Data region (if needed)
     */
    DATA            (R)   : origin = 0x80000000, length = 0x00007FFF
}

SECTIONS
{
    /* Interrupt vector table - must be at start of application FLASH */
    .intvecs: palign(16) {} > 0x10080000

    /* Code sections */
    .text   : palign(16) {} > FLASH
    .const  : palign(16) {} > FLASH
    .cinit  : palign(16) {} > FLASH
    .pinit  : palign(16) {} > FLASH
    .rodata : palign(16) {} > FLASH

    /* ARM exception handling */
    .ARM.exidx    : palign(16) {} > FLASH
    .init_array   : palign(16) {} > FLASH
    .binit        : palign(16) {} > FLASH

    /* RAM functions (if any code needs to run from RAM) */
    .TI.ramfunc   : load = FLASH, palign(16), run=SRAM, table(BINIT)

    /* Data sections in SRAM */
    .vtable :   > SRAM
    .args   :   > SRAM
    .data   :   > SRAM
    .bss    :   > SRAM
    .sysmem :   > SRAM
    .stack  :   > SRAM (HIGH)

    /* Data bank section */
    .DataBank : palign(16) {} > DATA
}
