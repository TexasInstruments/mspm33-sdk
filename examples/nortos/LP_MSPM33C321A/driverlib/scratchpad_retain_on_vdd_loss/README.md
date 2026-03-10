## Example Summary

Writes data to scratchpad memory that will be retained when VDD power is lost but VBAT is still supplied.

Scratch pad memory is a 32-byte register based memory, which will retain data similar to a non-volatile memory as long as VBAT is supplied. This means the scratch pad memory retains data when the main VDD supply is lost or during the SHUTDOWN mode. However, the memory content won't be retained, when the VBAT domain itself is lost.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| SYSCTL | PA3 | LFXIN |
| SYSCTL | PA4 | LFXOUT |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA3 | SYSCTL | LFXIN | N/A | <ul><li>PA3 is used for LFXT functionality</ul> |
| PA4 | SYSCTL | LFXOUT | N/A | <ul><li>PA4 is used for LFXT functionality</ul> |
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

The example performs the following operations:
1. Writes random data to the 32-byte scratchpad memory
2. Enables write protection for each byte
3. Waits for VDD power loss (device enters low-power mode)
4. Upon power restoration, verifies that all scratchpad memory bytes were retained

Compile, load and run the example.

Run the example first with `gVDDLost` set to false. This will first write data into
scratchpad memory. Disconnect VDD (3.3v jumper) from the system but leave VBAT powered.
Reconnect VDD power and re-run the example with `gVDDLost` set to true. The example
will now read back the data inside of scratchpad memory and verify that the data was retained
throughout the loss of VDD. A breakpoint will be hit and the value of `gErrorOccured` can be inspected. If all scratchpad memory was properly retained, `gErrorOccured` will be set to false and the green LED will turn on. Otherwise, `gErrorOccured` will be true if any data was not retained and the red LED will turn on.
