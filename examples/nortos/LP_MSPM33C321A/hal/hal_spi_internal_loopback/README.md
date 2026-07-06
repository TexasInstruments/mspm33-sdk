## Example Summary

SPI internal loopback example for MSPM33 using the HAL layer. Demonstrates polling-based synchronous transfers with hardware loopback (no external wiring required), data verification, and LED status indication.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| UC2 (SPI Controller) | PB18 | SCLK |
| UC2 (SPI Controller) | PB17 | PICO |
| UC2 (SPI Controller) | PB19 | POCI |
| UC2 (SPI Controller) | PA2 | CS0 |
| GPIOC | PC26 | LED (Red - Fail) |
| GPIOC | PC27 | LED (Green - Pass) |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

**Note:** No external jumpers required — loopback is done inside SPI hardware (PICO → POCI internally).

## Device Migration Recommendations

This project was developed for a superset device included in the LP_MSPM33C321A LaunchPad. Please visit the [CCS User's Guide](https://software-dl.ti.com/mspm33/esd/MSPM33-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration) for information about migrating to other MSPM33 devices.

## Example Usage

No external wiring needed. The example transmits 256 bytes `[0x00, 0x01, 0x02 ... 0xFF]` via SPI controller (UC2) with internal loopback enabled. The transmitted data is routed internally from PICO to POCI, received back, and verified using `memcmp()`.

Compile, load and run. LED behavior: Green (PC27) lights on success, Red (PC26) lights on any failure or data mismatch. Program breaks at breakpoint in both cases.

Configuration (in `app_cfg.h`):
- Instance: UC2
- Data Width: 8-bit
- Bit Rate: 500 kHz
- Buffer Size: 256 bytes
- Frame Format: Motorola 4-wire (POL0, PHA0)
