## Example Summary

UART loopback example for MSPM33 using the HAL layer. Demonstrates data transfer between two UART instances on a single board via physical jumper wires, with polling TX and interrupt RX modes.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| UC1_0 (TX Module) | PA0 | TX |
| UC1_0 (TX Module) | PA1 | RX |
| UC1_1 (RX Module) | PA15 | RX |
| UC1_1 (RX Module) | PA16 | TX |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

**Jumper Wiring (REQUIRED):**
- PA0 (UC1_0 TX) ↔ PA15 (UC1_1 RX)
- PA1 (UC1_0 RX) ↔ PA16 (UC1_1 TX)

## Device Migration Recommendations

This project was developed for a superset device included in the LP_MSPM33C321A LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/mspm33/esd/MSPM33-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM33 devices.

## Example Usage

Install both jumper wires (PA0↔PA15 for TX/RX, PA1↔PA16 for RX/TX). The example sends "Hello World !" from UC1_0 (polling mode TX) through the jumper wires to UC1_1 (interrupt mode RX), then verifies the received data matches.

Compile, load and run. LED behavior: Green (PA2) lights on success, Red (PC26) lights on any failure. Program breaks at breakpoint in both cases.

Configuration (in `app_cfg.h`):
- Baud Rate: 9600 bps
- Clock: 32 MHz
- TX Instance: UC1_0
- RX Instance: UC1_1
