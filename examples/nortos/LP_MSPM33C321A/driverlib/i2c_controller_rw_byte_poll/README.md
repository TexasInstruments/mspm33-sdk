## Example Summary

I2C Controller writes and reads multiple bytes to/from a target device.
The example uses non-burst mode and polling, sending one byte per transaction.
UC13_0 does not support burst mode, and this example demonstrates how to perform
I2C transfers without burst support.
The Target device must be enabled and active before running this example. The target device should run i2c_target_rw_byte_poll example.

This example can be used to validate I2C communication, as a starting point to
control any I2C target, or to create custom drivers.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output (Red LED) |
| GPIOC | PC27 | Standard Output (Green LED) |
| SYSCTL |  |  |
| UC13_0 | PB8 | I2C Serial Data line (SDA) |
| UC13_0 | PB9 | I2C Serial Clock line (SCL) |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PB8 | UC13_0 | SDA | J4_19 | N/A |
| PB9 | UC13_0 | SCL | J4_36 | N/A |
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

Connect SDA and SCL between I2C Controller and Target.
Compile, load and run the example.
The controller sends some data to the target and expects to receive some data from the target. If it receives the data, it will match with the expected data and if they match, the PASS_LED will be set or else, the FAIL_LED will be set.
