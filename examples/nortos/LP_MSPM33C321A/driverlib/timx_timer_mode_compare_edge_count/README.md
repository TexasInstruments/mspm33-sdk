## Example Summary

This example configures timer TIMG4_0 in edge count mode to detect a
predefined number of rising edges (5) on the input pin.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Digital Output (USER_LED_1) |
| GPIOC | PC16 | Digital Output (USER_TEST) |
| SYSCTL |  |  |
| TIMG4_0 | PB10 | Capture/Compare Pin 0 |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | USER_LED_1 | LED1 | N/A |
| PC16 | GPIOC | USER_TEST | J1_35 | N/A |
| PB10 | TIMG4_0 | CCP0 | J1_2 | N/A |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for the MSPM33C321A device included in the LP_MSPM33C321A LaunchPad.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage
Connect an edge generator source to PB10 (TIMG4_0 CCP0 pin). This can be a function generator
or a simple momentary switch.

Compile, load and run the example.

LED1 will toggle every time TIMG4_0 detects 5 rising edges on the input pin.

The USER_TEST pin (PC16) will mirror the behavior of LED1 and can be used
for monitoring the output with test equipment.
