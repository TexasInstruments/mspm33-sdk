## Example Summary

This example demonstrates how to use the Real-Time Clock (RTC_A) offset calibration feature on the MSPM33 microcontroller. The RTC_A is configured to use the Low-Frequency External Crystal (LFXT) as its clock source, and the example allows for runtime adjustment of the RTC_A calibration value to improve clock accuracy.

The calibration value can be adjusted within a range of -240 to +240 PPM (parts per million) using the onboard buttons. This adjustment can be used to compensate for crystal inaccuracies and improve the RTC_A's timekeeping precision.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA18 | USER_SWITCH_1 (Button 1) |
| GPIOC | PC17 | USER_SWITCH_2 (Button 2) |
| RTC_A | PA9 | RTC_A Output |
| LFXT | PA3 | LFXT Input |
| LFXT | PA4 | LFXT Output |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA18 | GPIOA | USER_SWITCH_1 | N/A | <ul><li>PA18 is connected to Button 1<br><ul><li>Used to increase calibration value</ul></ul> |
| PC17 | GPIOC | USER_SWITCH_2 | N/A | <ul><li>PC17 is connected to Button 2<br><ul><li>Used to decrease calibration value</ul></ul> |
| PA9 | RTC_A | RTC_A OUT | N/A | <ul><li>RTC_A output signal</ul> |
| PA3 | LFXT | LFXIN | N/A | <ul><li>LFXT input (32.768 kHz crystal)</ul> |
| PA4 | LFXT | LFXOUT | N/A | <ul><li>LFXT output (32.768 kHz crystal)</ul> |
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
The output frequency for offset calibration is by default set to 512Hz.
Connect external equipment such as oscilloscope to RTC_A_OUT (PA9).
Press S1 to increment offset by +1PPM, S2 to decrement correction by -1PPM.
