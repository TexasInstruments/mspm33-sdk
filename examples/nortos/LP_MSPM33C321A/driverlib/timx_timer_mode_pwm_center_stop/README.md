## Example Summary

This example configures timer TIMG4_0 to output a pair of center aligned PWM
signals. The device is configured to wait in STOP mode.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| SYSCTL |  |  |
| TIMG4_0 | PB10 | Capture/Compare Pin 0 |
| TIMG4_0 | PB11 | Capture/Compare Pin 1 |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PB10 | TIMG4_0 | CCP0 | J1_13 | N/A |
| PB11 | TIMG4_0 | CCP1 | J1_14 | N/A |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging</ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging</ul> |

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage
Compile, load and run the example.

TimerG4_0 will output a pair of center aligned PWM signals on
PB10 and PB11 using the LFCLK source with a prescaler of 3.

PB10 is configured at a 75% duty cycle.
PB11 is configured at a 50% duty cycle.

Monitor the PWM signals on PB10 and PB11 using an oscilloscope.
