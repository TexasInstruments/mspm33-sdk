## Crypto SHA-256 Algorithm Example
This example demonstrates the calculation of SHA-256 hmac using the SHA hardware module with the following configuration:

- Algorithm: SHA-256
- Mode: HMAC
- Input: message[8*1024] where all bytes are 'a'
- Output: 256-bit (32-byte) hmac value
- Verification against known hmac value

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
- Configure for SHA-256 operation
- Compute hmac for input
- Compute the known hmac value using the ASCII value of the given input
- In memory(hex):
  0x61  0x61 0x61 0x61 0x61 0x61 0x61 0x61 ........ 
- SHA-256 HMAC is calculated for 8 kb of data
- Use HMAC key: {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
}
- Compare with known hmac value
- Indicate result:
LED1 ON = Hmac matches expected value
TEST_PIN ON = Hmac verification failed
