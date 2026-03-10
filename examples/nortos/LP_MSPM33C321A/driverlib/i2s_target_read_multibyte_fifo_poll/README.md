## Example Summary

I2S Target waits to receive data from the controller and checks the received data against expected values.
The example uses FIFO and polling.

The controller will have i2s_controller_write_multibyte_fifo_poll example.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output (FAIL_LED) |
| GPIOC | PC27 | Standard Output (PASS_LED) |
| SYSCTL |  |  |
| I2S1 | PB20 | I2S Audio Data 0 (AD0) |
| I2S1 | PB23 | I2S Bit Clock (BCLK) |
| I2S1 | PB22 | I2S Word Clock (WCLK) |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC26 | GPIOC | FAIL_LED | J8_72 | <ul><li>This pin is used to indicate test failure<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul> |
| PC27 | GPIOC | PASS_LED | J8_71 | <ul><li>This pin is used to indicate test success<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul>|
| PB20 | I2S1 | AD0 | J8_43 | N/A |
| PB23 | I2S1 | BCLK | J8_46 | N/A |
| PB22 | I2S1 | WCLK | J8_45 | N/A |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |


### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage

Connect I2S pins (AD0, BCLK, WCLK) between I2S Controller and Target.
Compile, load and run the example.

The target receives data from the controller through AD0 pin. The target compares the received data with the expected data and if it matches, the PASS_LED will be set; otherwise, the FAIL_LED will be set.
