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
 * This file is to add a custom overridden definition for `strcmp` which is safe for M33 with no DSP extension.
 *
 * This is because the the `strcmp` implementation from the standard library (libc.a) of TI Arm Clang compiler for M33
 * uses DSP instructions like `UDDA8` in `strcmp_t2.S` This will result in "Undefined Instruction" - "Usage Fault".
 *
 * TODO: Remove this once the compiler supports this natively (CODEGEN-14889).
 */

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
/*                 Private Variable Declaration                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Public Functions                                           */
/* ========================================================================== */

#if __ARM_FEATURE_DSP != 1

int StrCmp_No_Dsp_Safe(const char* str1, const char* str2)
{
    while ((*str1 != '\0') && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return (int)((unsigned char)*str1 - (unsigned char)*str2);
}

int strcmp(const char* str1, const char* str2) __attribute__((alias("StrCmp_No_Dsp_Safe")));

#else /* #if __ARM_FEATURE_DSP != 1 */

// Dummy symbol to avoid empty object file and archive warning
int StrCmp_No_Dsp_Dummy(void)
{
    return 0;
}

#endif /* #else __ARM_FEATURE_DSP != 1 */

/* ========================================================================== */
/*                 Private Functions                                          */
/* ========================================================================== */

/* ========================================================================== */
/*                 Private Callback Handlers                                  */
/* ========================================================================== */

/* ========================================================================== */
/*                 Test/Debug/Other Sections                                  */
/* ========================================================================== */
