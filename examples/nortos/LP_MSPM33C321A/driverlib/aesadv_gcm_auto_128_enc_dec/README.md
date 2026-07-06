## Example Summary

The following example performs 256-bit AES GCM encryption and decryption, in GCM autonomous mode.
using the on-board AES engine with a software wrapper. It also performs Hide Protection on the keys using Global Security Controller(GSC).

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output |
| GPIOC | PC27 | Standard Output |
| SYSCTL |  |  |
| EVENT |  |  |
| AES |  |  |
| KEYSTORECTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
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
The application will perform AES GCM encryption followed by decryption, then hit a SW breakpoint to allow the user to inspect the value of gCorrectResults to see the results of the golden value comparison. If the decrypted output matches the original input, LED2 Green (PASS_LED) will turn on. If there is a mismatch, LED2 Red (FAIL_LED) will turn on instead.

> NOTE: Once the application has executed and has successfully applied hide protection on a sector, users will not be able to read/write to the sector(s) again, which also means that the user will be unable to flash another application, as long as the GSC configuration remains. In order to reset the GSC configuration, the user must perform a system reset(NRST), and then attempt to load other applications.
