## Example Summary

Note: The use of "Master" and "Slave", along with "MOSI/MISO" terminology is being considered obsolete. These terms will be replaced with "Controller" and "Peripheral", and "PICO/POCI" respectively.

The following example configures the SPI as a Peripheral.
This example can be used with the spi_controller_register_format example running on another device. Ensure that the SPI Peripheral is running before starting the SPI Controller example.
This example receives "register format" commands from the SPI Controller. The first byte transmitted by the SPI Controller is the command/register address.
The SPI Controller can send two different types of commands: CMD_WRITE_TYPE_X and CMD_READ_TYPE_X.

When the Controller sends CMD_WRITE_TYPE_X commands, the SPI Controller will then send data to the Peripheral to write to its registers. The Peripheral will initialize itself to receive gCmdWriteTypeXBuffer example buffers.
After all the data is received by the Peripheral, the received data will be stored in its corresponding gCmdWriteTypeXBuffer.

When the Controller sends CMD_READ_TYPE_X commands, the SPI Controller will read data from the Peripheral. The Peripheral will send example gCmdReadTypeXBuffer buffers in response.
After all the data is received by the Controller, the received data will be stored in its corresponding gCmdReadTypeXBuffer.

After a successful transaction, the PASS_LED will toggle its state, then the Peripheral
will go to SLEEP.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output |
| GPIOC | PC27 | Standard Output |
| SYSCTL |  |  |
| UC2 | PB18 | SPI SCLK (Clock) |
| UC2 | PB17 | SPI PICO (Peripheral In, Controller Out) |
| UC2 | PB19 | SPI POCI (Peripheral Out, Controller In) |
| UC2 | PA2 | SPI CS0 (Chip Select 0) |
| UC2 | PB24 | SPI CS3/CD (Chip Select 3/Command Data) |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC26 | GPIOC | PC26 | J8_72 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul> |
| PC27 | GPIOC | PC27 | J8_71 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul>|
| PB18 | UC2 | SCLK | J5_47 | N/A |
| PB17 | UC2 | MOSI | J6_55 | N/A |
| PB19 | UC2 | MISO | J6_54 | N/A |
| PA2 | UC2 | CS0 | J6_53 | N/A |
| PB24 | UC2 | CS3_CD_MISO3 | J7_66 | N/A |
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
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage
Make the following connections between the SPI Controller and SPI Peripheral:
- Controller SCLK -> Peripheral SCLK
- Controller PICO -> Peripheral PICO
- Controller POCI <- Peripheral POCI
- Controller CS   -> Peripheral CS

The SPI is initialized with the following configuration:
- SPI Peripheral
- Motorola 4 Wire with Polarity 0, Phase 0
- No parity
- 8 bits per transfer
- MSB first

Compile, load and run the example before running the Controller example.
The SPI will automatically start to transmit and receive data responding to
requests from the SPI Controller.

The LED will toggle after completion to indicate success.
USER_TEST_PIN GPIO will mimic the behavior of the LED pin on the BoosterPack
header and can be used to verify the LED behavior.
