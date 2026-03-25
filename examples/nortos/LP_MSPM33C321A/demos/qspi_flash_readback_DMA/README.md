## Example Summary

Note: The use of "Master" and "Slave", along with "MOSI/MISO" terminology is being considered obsolete. These terms will be replaced with "Controller" and "Peripheral", and "PICO/POCI" respectively.

The following example demonstrates DMA-based read and write operations with an external QSPI flash memory device using the QSPIFLASH driver library in QPI (Quad Peripheral Interface) mode.

This example initializes the QSPI peripheral and uses DMA transfers for efficient data movement. The QSPI is configured at 40 MHz clock speed (derived from 160 MHz CPUCLK via SYSPLL). The application uses the QSPIFLASH driver library to communicate with an external NOR flash memory chip.

The example performs the following operations:

1. **Initialize QSPI Driver**: Sets up the QSPIFLASH driver handle and parameters.

2. **Read JEDEC ID**: Reads the flash device ID (manufacturer ID, memory type, capacity) in SPI half-duplex mode to verify proper communication.

3. **Enable Quad Mode**: Sets the QE (Quad Enable) bit in the flash status register and enables QPI mode for 4-bit data transfers.

4. **Sector Erase**: Performs a sector erase operation at address 0x00001000 in QPI half-duplex mode.

5. **Page Program with DMA**: Programs 512 bytes of test data (256 x 16-bit words spanning 2 pages) using DMA transfers in QPI half-duplex mode. The QSPIFLASH library automatically handles page boundaries.

6. **Read with DMA**: Reads back the 512 bytes from the same address using Quad I/O Read command with DMA in QPI mode.

7. **Disable QPI Mode**: Returns the flash to standard SPI mode.

8. **Verify Data**: Compares the programmed data against the read data to verify data integrity.

Upon completion, the application lights the PASS_LED (PC27) if all data matches correctly, or lights the FAIL_LED (PC26) if any mismatch is detected, providing immediate visual feedback of the flash memory operation success.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | PASS_LED - Standard Output |
| GPIOC | PC26 | FAIL_LED - Standard Output |
| SYSCTL |  | SYSPLL configured for 160 MHz CPUCLK |
| DMA0 | CH0 | QSPI RX DMA Channel |
| DMA0 | CH1 | QSPI TX DMA Channel |
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
| PC27 | GPIOC | PC27 (PASS_LED) | J8_71 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul> |
| PC26 | GPIOC | PC26 (FAIL_LED) | J8_72 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul>|
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
to communicate with the external flash device.

Once all operations complete and data verification passes, the PASS_LED will turn on. If any operation fails or data mismatch is detected, the FAIL_LED turns on.

The QSPI is initialized with the following configuration:
- QSPI Controller mode
- Motorola 4 Wire with Polarity 0, Phase 0
- No parity
- 8 bits per transfer (16-bit word size for DMA operations)
- MSB first
- Chip Select 0
- Clock speed: 40 MHz (CPUCLK 160 MHz / 4)
- DMA-based transfers for both TX and RX
- QPI mode enabled for quad data transfers
