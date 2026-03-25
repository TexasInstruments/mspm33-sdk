## Example Summary

This example configures the UART as a LIN Commander, and demonstrates basic
transmit and receive of LIN 2.0 packets using enhanced checksum.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA18 | Standard Input (USER_SWITCH_2) |
| GPIOB | PB15 | Standard Output (LIN_ENABLE) |
| GPIOB | PB21 | Standard Input with internal pull-up (USER_SWITCH_1) |
| GPIOC | PC26 | Standard Output (USER_LED_2 Red) |
| GPIOC | PC27 | Standard Output (USER_LED_1 Green) |
| SYSCTL |  |  |
| TIMG4_0 |  | Timer for LIN timeout |
| UC1_0 | PA1 | RX Pin |
| UC1_0 | PA0 | TX Pin |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA18 | GPIOA | PA18 | N/A | <ul><li>PA18 is used as USER_SWITCH_2 input with rising edge interrupt</ul> |
| PB15 | GPIOB | PB15 | N/A | <ul><li>PB15 is used to enable the LIN transceiver</ul> |
| PB21 | GPIOB | PB21 | N/A | <ul><li>PB21 is used as USER_SWITCH_1 input with internal pull-up and falling edge interrupt</ul> |
| PC26 | GPIOC | PC26 | N/A | <ul><li>PC26 is connected to LED2 Red</ul> |
| PC27 | GPIOC | PC27 | N/A | <ul><li>PC27 is connected to LED2 Green</ul> |
| PA1 | UC1_0 | RX | N/A | <ul><li>PA1 is used as LIN UART RX with internal pull-up</ul> |
| PA0 | UC1_0 | TX | N/A | <ul><li>PA0 is used as LIN UART TX</ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging</ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging</ul> |

### Device Migration Recommendations


### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/slau962).

## Example Usage

### Hardware Connection Between LaunchPad and LIN Transceiver

The LaunchPad connects to a TLIN1021DEVM LIN transceiver module to form a communication unit. This unit can then be connected to another LIN node (such as another LaunchPad configured as a LIN Responder) or a LIN Analyzer.


**Connection Steps:**

1. Connect LaunchPad UART pins to TLIN1021DEVM:
   - PA0 (TX) -> TLIN1021DEVM TXD
   - PA1 (RX) -> TLIN1021DEVM RXD

2. Connect LaunchPad Power pins (3.3V and GND) to TLIN1021DEVM.

3. Connect LaunchPad PB15 (LIN_ENABLE) to TLIN1021DEVM EN pin.

4. Use external 12V power supply to TLIN1021DEVM VBAT pin. If the other connected communication unit (like a LIN Analyzer) already provides 12V, this step can be skipped.

5. Connect VBAT, LIN and GND pins on TLIN1021DEVM to the responder or other LIN node hardware.


The LIN Commander is configured to run at 19200 baud. These settings
can be modified in the application.

The list of acceptable PID commands are:
  - PID 0x39 / 0x3A / 0x3B
      - Usage: LIN Commander sends a packet containing 8 bytes of data for LIN Responder to receive and store.
      - Packet: [PID, DATA1, DATA2, DATA3, DATA4, DATA5, DATA6, DATA7, DATA8, ENHANCED_CHECKSUM]
      - Example Packet: [0x39, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0xA2]
  - PID 0x08 / 0x09 / 0x0D
      - Usage: LIN Commander tells the LIN Responder to respond with 5 bytes of stored data.
      - Packet: [PID]
      - Example Packet: [0x08]
The PIDs, message size, and functionality of the callback handlers can be modified
and customized in the "gCommanderMessageTable" array in lin_commander.c.

Compile, load and run the example.

When USER_SWITCH_1 (PB21) is pressed (failing edge), the following LIN 2.0 packet will be transmitted:
  - PID: 0x39
  - Data: [0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8]
  - Enhanced Checksum: 0xA2
Each time the USER_SWITCH_1 button is pressed, LED1 (Green) will toggle and the value of the first and last data bytes will be incremented by one.

When USER_SWITCH_2 (PA18) is pressed (rising edge), the following LIN 2.0 packet will be transmitted:
  - PID: 0x08
  - Data: None
  - Enhanced Checksum: None

It is expected for the LIN Commander to receive 5 bytes of data in response to PID 0x08.

If the LIN Responder correctly sends a response, then LED2 (Red) will toggle and the variable "gDataReceived" will be set to true.

The received bytes will be stored in "gCommanderRXBuffer". Each time a new packet of data is received, it will overwrite what was
previously stored in "gCommanderRXBuffer".
