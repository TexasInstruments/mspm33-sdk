## Example Summary

The following example configures the RTC_A in Calendar mode and enables Alarm 1.
The device remains in STANDBY mode while waiting for an interrupt.
The LED will be set when the alarm is triggered.


## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| GPIOA | PA27 | RTC_A Output |
| RTC_A |  |  |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA27 | RTC_A | RTC_A OUT | N/A | <ul><li>PA27 is the RTC_A output pin</ul> |
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
The RTC_A format used can be set by setting either DL_RTC_A_FORMAT_BINARY or
DL_RTC_A_FORMAT_BCD.
If DL_RTC_A_FORMAT_BINARY is set, binary format will be used.
If DL_RTC_A_FORMAT_BCD is set, BCD format will be used.
By default, the example uses binary format.

The RTC_A Calendar is initialized to the date April 5th, Monday, 2021 at
10:32:59 (HH:MM:SS).
The RTC_A Alarm 1 is set to trigger on the 5th of the month, Monday, 10:33.
Using these default configurations, the alarm will trigger 1 second after the
RTC_A clock is started.

Compile, load and run the example.
After 1 second, the LED will be set to indicate that the Alarm 1 was
successfully triggered. The example waits for interrupt and returns after the alarm is triggered.
