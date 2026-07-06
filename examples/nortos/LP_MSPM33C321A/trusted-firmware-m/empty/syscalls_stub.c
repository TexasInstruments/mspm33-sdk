/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 * Copyright (c) 2026, Texas Instruments Incorporated
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Stub implementations of newlib syscalls to suppress linker warnings
 * when building with GNU Arm toolchain >= 11.3.Rel1 and -specs=nosys.specs.
 */

__attribute__((weak)) void _close(void)
{
}
__attribute__((weak)) void _fstat(void)
{
}
__attribute__((weak)) void _getpid(void)
{
}
__attribute__((weak)) void _isatty(void)
{
}
__attribute__((weak)) void _kill(void)
{
}
__attribute__((weak)) void _lseek(void)
{
}
__attribute__((weak)) void _read(void)
{
}
__attribute__((weak)) void _write(void)
{
}
