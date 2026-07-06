## Example Summary

This example shows detection of arbitration error in a Multi-Controller bus.
I2C Controller writes multiple bytes to a target device until an arbitration error occurs.
PASS_LED will be set ON if transmission is successful, FAIL_LED will remain ON after arbitration error is detected.
One or more additional I2C controllers are needed to cause an arbitration error.
The Target device must be enabled and active before running this example.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output (FAIL_LED) |
| GPIOC | PC27 | Standard Output (PASS_LED) |
| SYSCTL |  |  |
| UC15_0 | PA0 | I2C Serial Data line (SDA) |
| UC15_0 | PA1 | I2C Serial Clock line (SCL) |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC26 | GPIOC | PC26 | J8_72 | <ul><li>PC26 can be connected to LED2 Red<br><ul><li>Enable via `LED2RedEn` launchpad shortcut</ul></ul> |
| PC27 | GPIOC | PC27 | J8_71 | <ul><li>PC27 can be connected to LED2 Green<br><ul><li>Enable via `LED2GreenEn` launchpad shortcut</ul></ul> |
| PA0 | UC15_0 | SDA | J5_50 | N/A |
| PA1 | UC15_0 | SCL | J5_49 | N/A |
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

> **Note:** I2C requires pull-up resistors on SDA and SCL lines. If pull-up resistors are not present on the board, connect external pull-up resistors (typically 4.7kΩ to 3.3V) on the SDA and SCL pins before running this example.

## Example Usage

Connect SDA (PA0 / J5_50) and SCL (PA1 / J5_49) between I2C Controllers and Target(s).
Add external pull-up resistors (4.7kΩ recommended) from SDA and SCL to 3.3V.
Compile, load and run the example. The Target device must be active before running this example on controllers.
PASS_LED (PC27) will be set ON if transmission is successful.
FAIL_LED (PC26) will remain ON if an arbitration error is detected.
