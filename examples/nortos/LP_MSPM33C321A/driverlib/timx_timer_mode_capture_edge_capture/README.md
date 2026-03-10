## Example Summary

The following example configures timer TIMG4_0 to measure the time from the start of
the capture operation to the signal edge.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Output |
| GPIOC | PC16 | Output |
| SYSCTL |  |  |
| TIMG4_0 | PB10 | Capture/Compare Pin 0 |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | Output | LED2 (Green) | N/A |
| PC16 | GPIOC | Output | J2_35 | N/A |
| PB10 | TIMG4_0 | CCP0 | J1_34 | N/A |
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
Connect to PB10 (GPIO_CAPTURE_0_C0_PIN) to an edge generator source (it can
also be connected to a momentary switch).
Compile, load and run the example.
Generate a rising edge on PB10 (GPIO_CAPTURE_0_C0_PIN). This will cause the
application to hit the breakpoint instruction and the value of `edgeCapture` can
be inspected to determine the time when the edge occurred.

After the variable `edgeCapture` is inspected, the user can resume
execution and detect a new rising edge.

LED2 (Green) will toggle every time a rising edge is detected and USER_TEST_PIN GPIO
will mimic the behavior of the LED pin on the BoosterPack header and can be used
to verify the LED behavior.
