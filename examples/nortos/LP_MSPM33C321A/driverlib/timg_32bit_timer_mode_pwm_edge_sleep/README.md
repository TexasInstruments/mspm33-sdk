## Example Summary

This example configures a 32-bit TimerG (TIMG12_0) to output a pair of edge-aligned PWM
signals. The device is configured to wait in SLEEP mode while the timer continues to operate.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| SYSCTL |  |  |
| TIMG12_0 | PB13 | Capture/Compare Pin 0 |
| TIMG12_0 | PB14 | Capture/Compare Pin 1 |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PB13 | TIMG12_0 | CCP0 | J4_35 | N/A |
| PB14 | TIMG12_0 | CCP1 | J4_37 | N/A |
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
Compile, load and run the example.

The 32-bit timer (TIMG12_0) will output a pair of 62.5Hz edge-aligned PWM signals with
duty cycles of 75% (on PB13) and 50% (on PB14) respectively. The PWM frequency is determined
by the timer count value of 512000 with a 32MHz clock source.

While the PWM signals are being generated, the device will remain in SLEEP mode to demonstrate
low-power operation while maintaining timer functionality.
