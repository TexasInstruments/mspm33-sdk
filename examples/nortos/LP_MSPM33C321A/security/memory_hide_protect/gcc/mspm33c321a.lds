/*****************************************************************************

  Copyright (C) 2025 Texas Instruments Incorporated - http://www.ti.com/

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

MEMORY
{
    FLASH              (rx)  : ORIGIN = 0x10000000, LENGTH = 0x0000F000   /* 60KB for Flash Application */
    FLASH_PROTECTED    (rx)  : ORIGIN = 0x1000F000, LENGTH = 0x00000800   /* 2KB HDP-Protected Region */
    NSC_FLASH          (rx)  : ORIGIN = 0x1000F800, LENGTH = 0x00000800   /* 2KB for NSC Region */
    SRAM               (rwx) : ORIGIN = 0x30000000, LENGTH = 0x00010000   /* 64KB SRAM */
    DATA               (r)   : ORIGIN = 0x80000000, LENGTH = 0x00008000   /* Data Bank */
    BCR_CONFIG         (r)   : ORIGIN = 0x80101800, LENGTH = 0x00000400   /* BCR Config */
    BSL_CONFIG         (r)   : ORIGIN = 0x80101C00, LENGTH = 0x00000400   /* BSL Config */
}

/* Define stack and heap sizes */
__stack_size = 256;
__heap_size = 0;

SECTIONS
{
    .intvecs : ALIGN(16)
    {
        __INTVECS_BASE__ = .;
        KEEP(*(.intvecs))
        . = ALIGN(16);
    } > FLASH

    .text : ALIGN(16)
    {
        *(.text*)
        *(.rodata*)
        . = ALIGN(16);
        __init_array_start = .;
        KEEP(*(.init_array*))
        __init_array_end = .;
        . = ALIGN(16);
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(16);
    } > FLASH

    .ARM.extab : ALIGN(16)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(16);
    } > FLASH

    .ARM.exidx : ALIGN(16)
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
        . = ALIGN(16);
    } > FLASH

    /* RAM functions - code that executes from SRAM but is loaded from FLASH */
    .ramfunc : ALIGN(16)
    {
        __ramfunct_load__ = LOADADDR(.ramfunc);
        __ramfunct_start__ = .;
        *(.ramfunc*)
        . = ALIGN(16);
        __ramfunct_end__ = .;
    } > SRAM AT > FLASH

    /* HDP-Protected Sensitive Code/Data Section */
    .protected_code : ALIGN(16)
    {
        *(.protected_code*)
    } > FLASH_PROTECTED

    .protected_data : ALIGN(16)
    {
        *(.protected_data*)
    } > FLASH_PROTECTED

    /* Non-Secure Callable region for secure gateway veneers */
    .gnu.sgstubs : ALIGN(32)
    {
        *(.gnu.sgstubs*)
    } > NSC_FLASH

    .data : ALIGN(16)
    {
        __data_load__ = LOADADDR(.data);
        __data_start__ = .;
        *(.data*)
        . = ALIGN(16);
        __data_end__ = .;
    } > SRAM AT > FLASH

    .bss : ALIGN(16)
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(16);
        __bss_end__ = .;
    } > SRAM

    .heap (NOLOAD) : ALIGN(8)
    {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        . = . + __heap_size;
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } > SRAM

    .stack (ORIGIN(SRAM) + LENGTH(SRAM) - __stack_size) (NOLOAD) : ALIGN(8)
    {
        __stack_start__ = .;
        . = . + __stack_size;
        __stack_end__ = .;
        __StackTop = .;
        __STACK_END = .;
    } > SRAM

    /* Data Bank section */
    .DataBank : ALIGN(16)
    {
        *(.DataBank*)
    } > DATA

    /* BCR and BSL Configuration */
    .BCRConfig :
    {
        KEEP(*(.BCRConfig*))
    } > BCR_CONFIG

    .BSLConfig :
    {
        KEEP(*(.BSLConfig*))
    } > BSL_CONFIG
}

/* Provide standard symbols */
PROVIDE(_stack = __StackTop);
PROVIDE(__stack = __StackTop);
