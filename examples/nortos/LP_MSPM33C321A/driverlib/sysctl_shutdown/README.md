## Example Summary

Configures different power policies based on the configuration of two input
pins. Which allows user to prevent the device to indefinately stay in SHUTDOWN
mode since the serial wire debug (SWD) remain locked while the device is in
SHUTDOWN mode.

After entering SHUTDOWN mode, the device will exit when a high level is detected
on the wake-up pin. Then it will toggle LED2 for as many times as it has exited
SHUTDOWN mode before (i.e. LED2 will toggle once the first time it wakes up, two
times the second time it wakes-up and so on).

If a different power policy is selected via the configuration input pins. The
device will only remain in the selected power policy but will not wake-up via
wake-up pin.

The table below shows the power policy to be configured based on configuration
input configuration.

|config input 0      |config input 1      |Power Policy        |
|--------------------|--------------------|--------------------|
| High (default)     | High (default)     | SHUTDOWN           |
| High (default)     | Low (pin grounded) | STOP              |
| Low (pin grounded) | High (default)     | STANDBY0           |
| Low (pin grounded) | Low (pin grounded) | RUNSLEEP         |

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA1 | Standard with Wake Input |
| GPIOC | PC26 | Standard Output |
| GPIOC | PC27 | Standard Output |
| GPIOB | PB14 | Standard Input|
| GPIOB | PB9 | High-Drive Input|
| SYSCTL |  |  |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings


| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA1 | GPIOA | PA1 | J5_49 | Switch used to wake up the example from shutdown mode (cannot wake from other low power modes) |
| PB14 | GPIOA | PB14 | J2_14 | (Config1): Configuration pin |
| PB9 | GPIOB | PB9 | J2_19 | (Config0): Configuration pin |
| PC26 | GPIOC | PC26 | J8_72 | (Red): Blinks to indicate the number of times the program has entered and exited shutdown |
| PC27 | GPIOC | PC27 | J8_71 | (Green): Blinks once for normal wakeup |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for a superset device included in the LP_MSPM33C321A LaunchPad. Please
visit the CCS User's Guide for information about migrating to other MSPM33 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage

1. Shutdown state cannot be entered while the debugger is connected
2. Build the example in CCS and load the .out file
3. Disconnect the debugger to run the example
4. Connect three jumpers to the three pins (PB9, PB14, PA1)
5. To enter shutdown modes, apply positive voltage (3.3V or 5V) to these configuration pins
6. Disconnect the debugger to start the program
7. On initial startup, the green LED will blink once
8. The program will then enter shutdown mode (based on config pins input)
9. To wake the core from shutdown:
    1. Apply high voltage (3.3V or 5V) to PA1 briefly
    2. Return PA1 to GND (default state)
10. After first wakeup from shutdown, the red LED will blink once
11. The core will re-enter shutdown mode if config pins are still set for shutdown
12. Repeating the PA1 wake-up process will cause the red LED to blink twice (indicating second shutdown entry)
13. During or after red LED blinking, config pins can be changed to select a different low power mode
14. If the core enters any low power mode other than shutdown, PA1 cannot wake the core
15. Clock outputs can be observed by probing PA9 with a logic analyzer
16. The output clock is shown with a divider of 16 on PA9
