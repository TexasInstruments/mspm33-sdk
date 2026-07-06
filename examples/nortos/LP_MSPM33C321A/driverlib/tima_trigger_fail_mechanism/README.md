## Example Summary

The following example configures fault detection feature on TimerA.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | USER_LED_1 |
| GPIOC | PC16 | USER_TEST |
| TIMERFAULT0 | PA26 | Fault Pin 0 |
| TIMA0_0 | PB14 | Capture/Compare Pin 0 |
| TIMA0_0 | PA1 | Capture/Compare Pin 1 |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | USER_LED_1 | LED2 | N/A |
| PC16 | GPIOC | USER_TEST | Test Pin | N/A |
| PA26 | TIMERFAULT0 | FAULT0 | External Pin | N/A |
| PB14 | TIMA0_0 | CCP0 | External Pin | N/A |
| PA1 | TIMA0_0 | CCP1 | External Pin | N/A |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging</ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging</ul> |

### Device Migration Recommendations
This project was developed for the MSPM33C321A device. Please
visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM0-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)
for information about migrating to other MSPM33 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage
Make sure external fault pin logic level is low before running example. Compile,
load and run the example. Monitor TIMA0_0_C0 (PB14), TIMA0_0_C1 (PA1), and
External fault pin (PA26). Analyze fault reaction behavior described in
"Application Design Details" section.

## Application Design Details
TimerA is configured to generate edge align PWM signals on TIMA0_0_C0/TIMA0_0_C1 and
monitor fault conditions via an external fault pin.

A fault condition is detected when the fault pin state remains in high state for
more than 5 timer clocks. This examples enables the fault input filter to handle
glitches on the fault input and is configured for consecutive periods mode for
3 timer clocks. The fault pin input is passed by a synchronizer which can take
up to 2 additional timer clock to start detecting consecutive periods.

Once a fault is detected, the following actions occurr:

- TimerA is configured to set the PWM as follows:
    - TIMA0_0_C0 is set to high state
    - TIMA0_0_C1 is set to low state
- USER_LED_1 and USER_TEST pins will toggle every time a fault is detected


Once the fault condition is cleared (i.e. fault pin remains in low state for at
least 5 timer clocks). TimerA will resume operation as follows and will perform
all three operations listed below simultaneously:

- TIMA0_0_C0 is set to low state
- TIMA0_0_C1 is set to high state
- Timer counter resumes operation based on counter value after enable. For
  this example, the counter starts counting down from the pwm period value.
