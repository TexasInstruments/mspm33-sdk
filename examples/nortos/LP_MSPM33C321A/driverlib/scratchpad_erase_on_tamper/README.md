## Example Summary

Writes data to scratchpad memory that will be erased upon detection of a tamper event on tamper pin.
TamperIO 0 configured to detect tamper event upon a rising edge at that pin.

Scratch pad memory is a 32-byte register based memory, which will retain data similar to a non-volatile memory as long as VBAT is supplied. This means the scratch pad memory retains data when the main VDD supply is lost or during the SHUTDOWN mode. However, the memory content won't be retained, when the VBAT domain itself is lost.

If a tamper event is registered while the device is not powered by VDD, the clear action shall be stored in a register until the device regains power.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| TAMPERIO | PC14 | TamperIO 0 Input |
| RTCA | PA27 | RTC Output |
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
| PC14 | TAMPERIO | TIO0 | MikroBUS Interface.9 | <ul><li>TamperIO 0 input for tamper detection</ul> |
| PA27 | RTCA | RTCOUT | N/A | <ul><li>RTC output signal</ul> |
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
3. Enables tamper erase for each byte
4. Configures TamperIO 0 to detect a rising edge tamper event
5. Waits for a tamper event
6. Verifies that all scratchpad memory bytes were erased after the tamper event

Compile, load and run the example.

Generate a rising edge on the TamperIO_0 (PC14) pin by connecting it to 5V/3V pin . This will cause the
scratchpad memory to erase itself . If all scratchpad memory
was properly erased, `gErrorOccured` will be set to false and the green LED will turn on. 
Otherwise, `gErrorOccured` will be true if any data was not properly erased and the red LED will turn on.
