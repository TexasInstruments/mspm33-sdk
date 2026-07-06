## Example Summary

Configures RTC_A to generate a time stamp event when a tamper event is detected.
TamperIO 0 is configured to generate a tamper event upon a rising edge.

When a time stamp event happens, the whole RTC register from second till years is copied to the time stamp capture registers.

In case of multiple time stamp events, the time stamp capture registers are configured to hold the latest
time stamp event.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| RTCA | PA27 | RTC Output |
| TAMPERIO | PC14 | Tamper I/O 0 |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA27 | RTCA | RTCOUT | N/A | <ul><li>RTC output signal</ul> |
| PC14 | TAMPERIO | TIO0 | MikroBUS Interface.9 | <ul><li>Tamper I/O 0 input</ul> |
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

Generate a rising edge on the TAMPERIO_0 pin (PC14). This will cause a time stamp event
to occur. Upon detection of a time stamp event, the green LED will toggle equal to the number
of time stamp events that have occurred so far. The example waits for interrupt and does not abort.

The time stamp capture registers can be inspected following a tamper event to see the exact time when the tamper occurred.
