# TF-M Platform Support for TI LP_MSPM33C321A

This directory contains the platform-specific implementation for Texas Instruments LP_MSPM33C321A (MSPM33C321A) microcontroller for Trusted Firmware-M (TF-M).

## Platform Overview

The LP_MSPM33C321A is based on the ARM Cortex-M33 processor with TrustZone support, making it suitable for PSA Certified implementations.

### Key Features
- **CPU**: ARM Cortex-M33 with TrustZone
- **Architecture**: ARMv8-M Mainline
- **Main Flash**: 1 MB (two security aliases: NS `0x00000000`, S `0x10000000`)
- **Data Flash**: 32 KB (two security aliases: NS `0x80000000`, S `0x90000000`)
- **SRAM**: 256 KB (two security aliases: NS `0x20000000`, S `0x30000000`)
- **Security**: TrustZone enabled, Global Security Controller (GSC)
- **BL2**: Not supported — direct code execution, no bootloader

## Memory Configuration

### Flash Layout (no BL2, single-image boot)

| Region     | Address Range             | Size   | Security |
|------------|---------------------------|--------|----------|
| S Flash    | 0x1000_0000 - 0x1007_FFFF | 512 KB | Secure   |
| NS Flash   | 0x0008_0000 - 0x000F_FFFF | 512 KB | NS       |
| Data Flash | 0x8000_0000 - 0x8000_7FFF | 32 KB  | Secure   |

### SRAM Layout

| Region  | Address Range             | Size   | Security |
|---------|---------------------------|--------|----------|
| S SRAM  | 0x3000_0000 - 0x3001_FFFF | 128 KB | Secure   |
| NS SRAM | 0x2002_0000 - 0x2003_FFFF | 128 KB | NS       |

### Data Flash Layout

| Region             | Offset              | Size  | Contents                |
|--------------------|---------------------|-------|-------------------------|
| OTP / NV Counters  | 0x8000_0000         | 8 KB  | NV counters, OTP data   |
| Internal Trusted Storage (ITS) | 0x8000_2000 | 16 KB | Secure key/data storage |
| Protected Storage (PS)         | 0x8000_6000 | 8 KB  | NS-accessible storage   |

### Flash Programming
- Sector size (minimum erase unit): 4 KB
- Flash word width (programming unit): 128-bit (16 bytes) — all program addresses must be 16-byte aligned

## Implemented Peripherals

| Peripheral | Driver | Notes |
|------------|--------|-------|
| UART (UART0) | `cmsis_drivers/Driver_USART.c` | Debug/console output |
| Flash Controller | `cmsis_drivers/Driver_Flash.c` | Address-based dispatch between main and data flash |
| Global Security Controller (GSC) | `src/target_cfg.c` | SRAM and flash security attribution |

## Notes

- BL2 (MCUboot) is **not** supported; the secure image executes directly from `0x10000000`
- ITS, PS, and NV counters reside in data flash (Bank 2) to preserve main flash for code
- The CMSIS Flash driver (`Driver_FLASH0`) dispatches between main and data flash based on address: addresses `>= 0x80000000` go to data flash, all others to main flash
- SRAM security split is configured via the GSC in `target_cfg.c` (pages 0–7 Secure, pages 8–15 NS) and must match the sizes defined in `partition/flash_layout.h`

## License

Copyright (c) 2026 Texas Instruments Incorporated. All rights reserved.

SPDX-License-Identifier: BSD-3-Clause
