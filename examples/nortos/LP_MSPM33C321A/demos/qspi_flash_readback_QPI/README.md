## Example Summary

Note: The use of "Master" and "Slave", along with "MOSI/MISO" terminology is being considered obsolete. These terms will be replaced with "Controller" and "Peripheral", and "PICO/POCI" respectively.

The following example demonstrates read and write operations with an external QSPI flash memory device using QPI (Quad Peripheral Interface) mode.

This example initializes the QSPI peripheral and configures it to communicate with an external flash memory chip. The application begins by reading the flash device ID in SPI half-duplex mode to verify proper communication with the memory device.

After successful initialization, the example sets the QE (Quad Enable) bit in the flash status register and enables QPI mode. Once QPI mode is active, the flash device communicates using all four data lines (IO0-IO3) for both command and data phases, providing higher throughput compared to standard SPI mode.

The example performs a complete chip erase operation in QPI mode to clear all existing data in the flash memory. After the erase completes, the application programs a 256-byte test pattern (stored as 128 16-bit words) to address 0x00000000 using the page program command in QPI half-duplex mode.

Once the programming operation completes, the example reads back the 256 bytes from the same address location using Quad IO Read in QPI mode with configurable dummy cycles. The application then performs a byte-by-byte comparison between the original data buffer and the readback buffer to verify data integrity.

After verification, the example disables QPI mode to return the flash to standard SPI operation. Upon completion, the application lights the PASS_LED if all bytes match correctly, or lights the FAIL_LED if any byte mismatch is detected, providing immediate visual feedback of the flash memory operation success.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output |
| GPIOC | PC27 | Standard Output |
| SYSCTL |  |  |
| QSPI | PB16 | QSPI SCLK (Clock) |
| QSPI | PA12 | QSPI PICO (Peripheral In, Controller Out) |
| QSPI | PA14 | QSPI POCI (Peripheral Out, Controller In) |
| QSPI | PA13 | QSPI IO2 (Serial IO2) |
| QSPI | PB15 | QSPI IO3 (Serial IO3) |
| QSPI | PC0 | QSPI CS0 (Chip Select 0) |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC26 | GPIOC | PC26 | J8_72 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul> |
| PC27 | GPIOC | PC27 | J8_71 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul>|
| PB16 | QSPI | SCLK | J11 | N/A |
| PA12 | QSPI | PICO | J11 | N/A |
| PA14 | QSPI | POCI | J11 | N/A |
| PA13 | QSPI | IO2 | J11 | N/A |
| PB15 | QSPI | IO3 | J11 | N/A |
| PC0 | QSPI | CS0 | J11 | N/A |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for a superset device included in the LP_MSPM33C321A LaunchPad. Please
visit the [CCS User's Guide](https://software-dl.ti.com/mspm33/esd/MSPM33-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)
for information about migrating to other MSPM33 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide].

## Example Usage
Make the following connections between the QSPI Controller and External NOR Flash:
- Controller SCLK -> Peripheral SCLK
- Controller PICO -> Peripheral PICO
- Controller POCI <- Peripheral POCI
- Controller CS   -> Peripheral CS
- Controller QSPI_IO2 -> Peripheral WP#/SIO2
- Controller QSPI_IO3 -> Peripheral RESET#/SIO3
- Controller GND  -> Peripheral GND
- Controller 3V3  -> Peripheral VCC

Compile, load and run the example. The QSPI will automatically start
to transmit and receive data.

Once the data is received, if it matches with the programmed data, the PASS_LED will turn on, else the FAIL_LED turns on.

The QSPI is initialized with the following configuration:
- QSPI Controller
- Motorola 4 Wire with Polarity 0, Phase 0
- No parity
- 8 bits per transfer
- MSB first
- Chip Select 0
