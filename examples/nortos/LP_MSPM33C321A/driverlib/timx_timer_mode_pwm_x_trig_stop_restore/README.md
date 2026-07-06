
## Example Summary
This example demonstrates cross-triggering between two PWM timers and register retention during low-power modes.
Two PWMs are generated for 1 second, then the PWM timer configuration is saved
before going to STOP mode. The device stays in STOP mode for 1 second and then
restores PWM generation.

Some MSPM33 peripherals may not retain register contents when
entering STOP or STANDBY modes, refer to the datasheet for more details.
TimerA is a peripheral which exhibits this behavior.
Developers can decide to re-initialize the peripheral using the default
initialization from SysConfig. This approach is more memory-efficient.
Alternatively, this example demonstrates using DriverLib APIs to save and
restore the register configuration of the peripheral before and after entering
low-power mode. This approach is recommended if the Timer configuration is
modified at runtime.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Digital Output (USER_LED_1) |
| GPIOC | PC16 | Digital Output (USER_TEST) with internal pull-down |
| SYSCTL |  |  |
| TIMA0_0 | PB14 | Capture/Compare Pin 0 |
| TIMA0_0 | PB20 | Capture/Compare Pin 1 |
| TIMG4_2 | PB10 | Capture/Compare Pin 0 |
| TIMG4_2 | PB11 | Capture/Compare Pin 1 |
| TIMG4_0 |  | Timer for 1-second period |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | USER_LED_1 | LED2 (Green) | N/A |
| PC16 | GPIOC | USER_TEST | J1_35 | N/A |
| PB14 | TIMA0_0 | CCP0 | J1_31 | N/A |
| PB20 | TIMA0_0 | CCP1 | J1_37 | N/A |
| PB10 | TIMG4_2 | CCP0 | J1_27 | N/A |
| PB11 | TIMG4_2 | CCP1 | J1_28 | N/A |
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
Monitor PWM generation before and after STOP mode.
USER_TEST_PIN will go high when 1 sec timeout counter is started and go low
when the 1 sec period has completed. USER_LED_PIN will go high to turn on a LED
once the example finished running without an error.
