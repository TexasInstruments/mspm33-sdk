## Example Summary

This example configures the TamperIO 0 in output mode with a heartbeat signal.
The heartbeat mode is set to ALWAYS_ON, with a pulse width of 1 ms and an interval of 125 ms.
The green LED is toggled in the main loop to indicate the program is running.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| TAMPERIO | PC14 | TamperIO 0 Output |
| RTCA |  |  |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC14 | TAMPERIO | TIO0 | MikroBUS Interface.9 | <ul><li>TamperIO 0 output with heartbeat signal</ul> |
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

The TamperIO is configured with the following settings:
- TamperIO 0 is set to output mode with heartbeat signal
- Heartbeat mode is set to ALWAYS_ON
- Heartbeat pulse width is set to 1 ms
- Heartbeat interval is set to 125 ms (0.125 seconds)

Compile, load and run the example.
The heartbeat pulse can be measured on TAMPERIO0.
The TamperIO 0 pin will output a heartbeat signal with a pulse width of 1 ms every 125 ms.
The green LED (PASS_LED) will toggle every 500 ms while main power is supplied.
