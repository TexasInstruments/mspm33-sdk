## Example Summary

I2C controller-target loopback example for MSPM33 using the HAL layer. Demonstrates polling-based synchronous transfers between two I2C instances on a single board with data verification and error handling.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| UC1 (Controller) | PA0 | SDA |
| UC1 (Controller) | PA1 | SCL |
| UC13 (Target) | PB12 | SDA |
| UC13 (Target) | PB13 | SCL |
| GPIOA | PA2 | LED1 (Blue) |
| GPIOC | PC26 | LED2 (Red - Fail) |
| GPIOC | PC27 | LED3 (Green - Pass) |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

**Jumper Wiring (REQUIRED):**
- PA0 ↔ PB12 (SDA)
- PA1 ↔ PB13 (SCL)

## Device Migration Recommendations

This project was developed for a superset device included in the LP_MSPM33C321A LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/mspm33/esd/MSPM33-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM33 devices.

## Example Usage

Connect jumper wires between the two I2C instances (PA0↔PB12 for SDA, PA1↔PB13 for SCL). The example performs bidirectional transfers in polling mode:

1. Part 1: Controller writes 16 bytes → Target receives and verifies
2. Part 2: Target transforms data (cipher) → Controller reads with repeated START

Compile, load and run. LED behavior on success: Green (PC27) lights up. On any error: Red (PC26) lights up and program breaks.

Configuration (in `app_cfg.h`):
- Bus Speed: 100 kHz
- Target Address: 0x50 (7-bit)
- Buffer Size: 16 bytes
- Clock Stretching: Enabled
