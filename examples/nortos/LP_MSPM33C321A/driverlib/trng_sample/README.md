## Example Summary

The following example configures the TRNG peripheral to fill a buffer with
random data.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (LED2 Green) |
| GPIOC | PC26 | Standard Output (LED2 Red) |
| SYSCTL |  |  |
| EVENT |  |  |
| TRNG |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PC27 | LED2 Green | <ul><li>LED2 Green will indicate TRNG operation status</ul> |
| PC28 | GPIOC | PC26 | LED2 Red | <ul><li>LED2 Red will indicate TRNG operation status</ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging</ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging</ul> |

### Device Migration Recommendations
This project was developed for the MSPM33C321A device included in the LP_MSPM33C321A LaunchPad.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage
Compile, load and run the example. The LED2 (Green and Red) will indicate the status of the TRNG operation. 
The application will pause at a software breakpoint, where the user can verify that gTRNGBuffer has been 
filled with random data. After verification, the LED status will change to indicate completion.
