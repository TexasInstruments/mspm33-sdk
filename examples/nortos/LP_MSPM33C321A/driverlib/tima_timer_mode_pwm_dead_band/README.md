## Example Summary

This example configures TimerA0_0 in PWM mode with dead band functionality.
It sets up two PWM channels with different duty cycles (50% and 75%) and configures
dead band rise and fall delay times. Dead band insertion is useful in applications
like motor control where you need to prevent shoot-through current in H-bridge circuits.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| TIMA0_0 | PB14 | CCP0 Output |
| TIMA0_0 | PA22 | CCP0 Complementary Output |
| TIMA0_0 | PA1 | CCP1 Output |
| TIMA0_0 | PA25 | CCP1 Complementary Output |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PB14 | TIMA0_0 | CCP0 Output | J1_11 | <ul><li>PWM output with 50% duty cycle</ul> |
| PA22 | TIMA0_0 | CCP0 Complementary Output | J1_12 | <ul><li>Complementary PWM output with dead band</ul> |
| PA1 | TIMA0_0 | CCP1 Output | J2_19 | <ul><li>PWM output with 75% duty cycle</ul> |
| PA25 | TIMA0_0 | CCP1 Complementary Output | J1_15 | <ul><li>Complementary PWM output with dead band</ul> |
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

This example demonstrates PWM generation with dead band insertion:
- Uses TimerA0_0 with clock prescale of 256
- Timer count of 2000
- Dead band enabled with rise delay of 50 timer counts and fall delay of 25 timer counts
- Channel 0: 50% duty cycle on PB14 with complementary output on PA22
- Channel 1: 75% duty cycle on PA1 with complementary output on PA25

Connect an oscilloscope to observe the PWM signals and dead band insertion:
- Compare PB14 (CCP0) and PA22 (CCP0 complementary) to see the dead band insertion
- Compare PA1 (CCP1) and PA25 (CCP1 complementary) to see the dead band insertion

The dead band insertion ensures that there is a delay between one output turning off
and its complementary output turning on, which prevents shoot-through current in
applications like motor control H-bridges.
