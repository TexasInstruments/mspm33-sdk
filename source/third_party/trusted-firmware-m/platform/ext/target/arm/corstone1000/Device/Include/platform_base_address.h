/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This file is derivative of CMSIS V5.01:
 * Device\_Template_Vendor\Vendor\Device\Include\Device.h
 */

#ifndef __PLATFORM_BASE_ADDRESS_H__
#define __PLATFORM_BASE_ADDRESS_H__

/* Secure Enclave Peripheral base addresses */
#define CORSTONE1000_ROM_BASE                      (0x00000000U) /* SE ROM Region                     */
#define CORSTONE1000_CRYPTO_ACCELERATOR_BASE       (0x2F000000U) /* Crypto Accelerator                */
#define CORSTONE1000_SRAM_BASE                     (0x30000000U) /* SE RAM Region                     */
#define CORSTONE1000_PERIPHERAL_BASE               (0x50000000U) /* SE Peripheral Region              */
#define CORSTONE1000_CMSDK_TIMER_0_BASE            (0x50000000U) /* CMSDK Timer 0                     */
#define CORSTONE1000_CMSDK_TIMER_1_BASE            (0x50001000U) /* CMSDK Timer 1                     */
#define CORSTONE1000_SEH_0_SENDER_BASE             (0x50003000U) /* SEH0 Sender                       */
#define CORSTONE1000_HSE_0_RECEIVER_BASE           (0x50004000U) /* HSE1 Receiver                     */
#define CORSTONE1000_SEH_1_SENDER_BASE             (0x50005000U) /* SEH1 Sender                       */
#define CORSTONE1000_HSE_1_RECEIVER_BASE           (0x50006000U) /* HSE1 Receiver                     */
/* Not all of the SEESx/ESxSE peripherals will be applicable, depending on the
 * number of external systems present         */
#define CORSTONE1000_SEES0_0_SENDER_BASE           (0x50010000U) /* SEES0 0 Sender                    */
#define CORSTONE1000_ES0SE_0_RECEIVER_BASE         (0x50011000U) /* ES0SE 0 Receiver                  */
#define CORSTONE1000_SEES0_1_SENDER_BASE           (0x50012000U) /* SEES0 1 Sender                    */
#define CORSTONE1000_ES0SE_1_RECEIVER_BASE         (0x50013000U) /* ES0SE 1 Receiver                  */
#define CORSTONE1000_SEES1_0_SENDER_BASE           (0x50014000U) /* SEES1 0 Sender                    */
#define CORSTONE1000_ES1SE_0_RECEIVER_BASE         (0x50015000U) /* ES1SE 0 Receiver                  */
#define CORSTONE1000_SEES1_1_SENDER_BASE           (0x50016000U) /* SEES1 1 Sender                    */
#define CORSTONE1000_ES1SE_1_RECEIVER_BASE         (0x50017000U) /* ES1SE 1 Receiver                  */
#define CORSTONE1000_SEES2_0_SENDER_BASE           (0x50018000U) /* SEES2 0 Sender                    */
#define CORSTONE1000_ES2SE_0_RECEIVER_BASE         (0x50019000U) /* ES2SE 0 Receiver                  */
#define CORSTONE1000_SEES2_1_SENDER_BASE           (0x5001A000U) /* SEES2 1 Sender                    */
#define CORSTONE1000_ES2SE_1_RECEIVER_BASE         (0x5001B000U) /* ES2SE 1 Receiver                  */
#define CORSTONE1000_SEES3_0_SENDER_BASE           (0x5001C000U) /* SEES3 0 Sender                    */
#define CORSTONE1000_ES3SE_0_RECEIVER_BASE         (0x5001D000U) /* ES3SE 0 Receiver                  */
#define CORSTONE1000_SEES3_1_SENDER_BASE           (0x5001E000U) /* SEES3 1 Sender                    */
#define CORSTONE1000_ES3SE_1_RECEIVER_BASE         (0x5001F000U) /* ES3SE 1 Receiver                  */
#define CORSTONE1000_SCR_BASE                      (0x50080000U) /* System Control Register           */
#define CORSTONE1000_WATCHDOG_TIMER_BASE           (0x50081000U) /* Watchdog Timer                    */
#define CORSTONE1000_SECENCTOP_PPU_BASE            (0x5008D000U) /* SECENCTOP PPU                     */
#define CORSTONE1000_BASE_SCR_BASE                 (0x5008E000U) /* SE Base System Control Register   */
#define CORSTONE1000_SOC_WATCHDOG_BASE             (0x5008F000U) /* SoC Watchdog                      */
#define CORSTONE1000_UART_BASE                     (0x50090000U) /* UART                              */
#define CORSTONE1000_SE_FIREWALL_BASE              (0x50200000U) /* SE Firewall                       */
#define CORSTONE1000_HOST_ACCESS_REGION_BASE       (0x60000000U) /* Host Access Region                */
#define CORSTONE1000_PPB_BASE                      (0xE0000000U) /* Private Peripheral Bus (PPB)      */
#define CORSTONE1000_CS_ROM_BASE                   (0xF0000000U) /* SE CS ROM                         */
#define CORSTONE1000_CTI_BASE                      (0xF0001000U) /* SE CTI                            */

/* Host base addresses from the SE perspective - partial list, only the ones
 * required by the SE are defined here */
#define CORSTONE1000_HOST_ADDRESS_SPACE_BASE       (0x60000000U) /* Host Address Space                */

/* Registers offsets - partial list, only the ones
 * required by the SE are defined here */
#define CORSTONE1000_HOST_BIR_OFFSET                                           (0x00000000U)
#define CORSTONE1000_HOST_CVM_OFFSET                                           (0x02000000U)
#define CORSTONE1000_HOST_XNVM_OFFSET                                          (0x08000000U)
#define CORSTONE1000_HOST_BASE_SYSTEM_CONTROL_BASE_OFFSET                      (0x1A010000U)
#define CORSTONE1000_HOST_EXT_SYS0_RST_CTRL_OFFSET                             (0x310U)
#define CORSTONE1000_HOST_FIREWALL_BASE_OFFSET                                 (0x1A800000U)
#define CORSTONE1000_HOST_INT_APBCOM_BASE_OFFSET                               (0x1B900000U)
/* The below offsets have been derived from the reserved section.*/
#define CORSTONE1000_HOST_FPGA_SCC_REGISTERS_OFFSET                            (0x20000000U)
#define CORSTONE1000_HOST_SE_SECURE_FLASH_BASE_FVP_OFFSET                      (0x20010000U)
#define CORSTONE1000_HOST_AXI_QSPI_CTRL_REG_BASE_OFFSET                        (0x20050000U)
#define CORSTONE1000_HOST_AXI_QSPI_CTRL_REG_BASE_SE_SECURE_FLASH_OFFSET        (0x30010000U)
#define CORSTONE1000_HOST_DRAM_UEFI_CAPSULE_OFFSET                             (0x40000000U)
#define CORSTONE1000_HOST_DSU_120T_BASE_OFFSET                                 (0x60910000U)

/* Register Addresses */
#define CORSTONE1000_HOST_BIR_BASE                 (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_BIR_OFFSET)                                                  /* Boot Instruction Register */
#define CORSTONE1000_HOST_TRUSTED_RAM_BASE         (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_CVM_OFFSET)                                                  /* On-chip Volatile Memory */
#define CORSTONE1000_HOST_XNVM_BASE                (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_XNVM_OFFSET)                                                 /* eXecute-in-place Non-volatile Memory */
#define CORSTONE1000_HOST_BASE_SYSTEM_CONTROL_BASE (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_BASE_SYSTEM_CONTROL_BASE_OFFSET)                             /* Security Control Bits */
#define CORSTONE1000_EXT_SYS_RESET_REG             (CORSTONE1000_HOST_BASE_SYSTEM_CONTROL_BASE + CORSTONE1000_HOST_EXT_SYS0_RST_CTRL_OFFSET)                              /* External System (Cortex-M3/NPU) */
#define CORSTONE1000_HOST_FIREWALL_BASE            (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_FIREWALL_BASE_OFFSET)                                        /* System Firewall */
#define CORSTONE1000_HOST_INT_APBCOM_BASE          (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_INT_APBCOM_BASE_OFFSET)                                      /* Internal Advanced Peripheral Bus Communication */
#define CORSTONE1000_HOST_FPGA_SCC_REGISTERS       (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_FPGA_SCC_REGISTERS_OFFSET)                                   /* FPGA Serial Communication Controller Registers */
#define CORSTONE1000_HOST_SE_SECURE_FLASH_BASE_FVP (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_SE_SECURE_FLASH_BASE_FVP_OFFSET)                             /* Secure Enclave Flash */
#define CORSTONE1000_HOST_AXI_QSPI_CTRL_REG_BASE   (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_AXI_QSPI_CTRL_REG_BASE_OFFSET)                               /* AXI QSPI Controller */
#define CORSTONE1000_HOST_AXI_QSPI_CTRL_REG_BASE_SE_SECURE_FLASH (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_AXI_QSPI_CTRL_REG_BASE_SE_SECURE_FLASH_OFFSET) /* AXI QSPI Controller for SE FLash  */
#define CORSTONE1000_HOST_DRAM_UEFI_CAPSULE        (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_DRAM_UEFI_CAPSULE_OFFSET)                                    /* DDR RAM (1.5 GB) */
#ifdef CORSTONE1000_DSU_120T
#define CORSTONE1000_HOST_DSU_120T_BASE            (CORSTONE1000_HOST_ADDRESS_SPACE_BASE + CORSTONE1000_HOST_DSU_120T_BASE_OFFSET)                                        /* DynamIQ Shared Unit-120 Power Policy Units */
#endif

/* Bit definition for External System Reset Control register */
#define CORSTONE1000_EXT_SYS_RESET_REG_CPUWAIT_Pos           (0U) // CPU Wait Control. 0b0 - External System's CPUWAIT signal is de-asserted. 0b1 - External System's CPUWAIT signal is asserted.
#define CORSTONE1000_EXT_SYS_RESET_REG_CPUWAIT_Msk           (0x1UL << CORSTONE1000_EXT_SYS_RESET_REG_CPUWAIT_Pos)  // 1 bit
#define CORSTONE1000_EXT_SYS_RESET_REG_RST_REQ_Pos           (1U) // Reset Request for External System. 0b0 - No Reset Requested. 0b1 - Reset Requested.
#define CORSTONE1000_EXT_SYS_RESET_REG_RST_REQ_Msk           (0x1UL << CORSTONE1000_EXT_SYS_RESET_REG_RST_REQ_Pos)  // 1 bit
#define CORSTONE1000_EXT_SYS_RESET_REG_Reserved_Pos          (2U) // Reserved.
#define CORSTONE1000_EXT_SYS_RESET_REG_Reserved_Msk          (0x3FFFFFFFUL << CORSTONE1000_EXT_SYS_RESET_REG_Reserved_Pos)  // 30 bits

/* Map Component definitions to Corstone definitions */
#define CC3XX_BASE_S        CORSTONE1000_CRYPTO_ACCELERATOR_BASE

#endif  /* __PLATFORM_BASE_ADDRESS_H__ */
