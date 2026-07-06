## Example Summary

The following example configures TimerA0_0 to output a single PWM signal and update
the signal’s frequency and duty cycle by using the shadow load and shadow capture
compare feature. Shadow load will only update the timer load values on a zero event
and the shadow capture compare value (which controls the PWM duty cycle) update
is configured to update when the timer reaches a count of zero.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Digital Output (USER_LED_1) |
| GPIOC | PC16 | Digital Output (USER_TEST) |
| SYSCTL |  |  |
| TIMA0_0 | PB14 | Capture/Compare Pin 0 (PWM Output) |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | USER_LED_1 | LED2 (Green) | N/A |
| PC16 | GPIOC | USER_TEST | J1_35 | N/A |
| PB14 | TIMA0_0 | CCP0 | J1_30 | N/A |
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

LED_1 will be off at the beginning of the program execution.
LED_1 will toggle each time the PWM period and duty cycle are updated.
USER_TEST_PIN GPIO will mimic the behavior of the LED pin on the
BoosterPack header and can be used to verify the LED behavior.

PWM signal on PB14.
