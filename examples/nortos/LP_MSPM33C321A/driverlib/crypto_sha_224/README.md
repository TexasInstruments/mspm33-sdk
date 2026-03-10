## Crypto SHA-224 Algorithm Example

This example demonstrates the calculation of SHA-224 hash using the SHA hardware module with the following configuration:

- Algorithm: SHA-224
- Mode: Hash (not HMAC)
- Input: message[8*1024] where all bytes are 'a'
- Output: 224-bit (28-byte) hash value
- Verification against known hash value

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| GPIOC | PC17 | Standard Input with interrupt (Button) |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC17 | GPIOC | USER_SWITCH_1 | N/A | <ul><li>PC17 is connected to Button S2<br><ul><li>Button S2 connects the pin to GND when pressed</ul></ul> |
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

Compile, load and run the example. The program will:

- Initialize the SHA hardware module
- Configure for SHA-224 operation
- Compute hash for input message[8*1024] where all bytes are 'a'
- Compute the known hash value using the ASCII value of the given input
- In memory(hex):
  0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 ........ 
- SHA-224 is calculated for 8 kb of data
- Compare with known hash value
- Indicate result:
LED1 ON = Hash matches expected value
TEST_PIN ON = Hash verification failed
