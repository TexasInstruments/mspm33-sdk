## Example Summary

This example demonstrates PIR (Passive Infrared) motion detection with live preview on EdgeAI Studio using MSPM33 devices. The application captures PIR sensor data via the ADC, extracts spectral features using FFT-based feature extraction, and performs inference using the NPU to classify motion into three categories:

- **Human Motion** - RED LED
- **Background Motion** - BLUE LED
- **Dog Motion** - GREEN LED

The classification results are streamed to EdgeAI Studio for real-time visualization.

This example uses software accelerated inference via the CPU. Please remove J3 to reduce noise on PA18.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| SYSCTL |  |  |
| UART0 | PA10 | TX Pin (Back-channel) |
| UART0 | PA11 | RX Pin (Back-channel) |
| GPIO | PC26 | LED RED |
| GPIO | PA2 | LED BLUE |
| GPIO | PC27 | LED GREEN |
| HSADC | ADC0 CH10 | PIR Sensor Input |
| TIMER | TIMA0_0 | ADC Sampling Timer |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA10 | UART0 | TX | J8_80 | <ul><li>PA10 can be used as UART TX for back-channel communication</ul> |
| PA11 | UART0 | RX | J8_79 | <ul><li>PA11 can be used as UART RX for back-channel communication</ul> |
| PC26 | GPIO | LED RED | J8_72 | <ul><li>This pin can be used for testing purposes</ul> |
| PA2 | GPIO | LED BLUE | J8_75 | <ul><li>This pin can be used for testing purposes</ul> |
| PC27 | GPIO | LED GREEN | J8_71 | <ul><li>This pin can be used for testing purposes</ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for a superset device included in the LP_MSPM33C321A LaunchPad. Please
visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM33-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)
for information about migrating to other MSPM33 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage

To initiate the live preview:
1. Configure the serial port in EdgeAI Studio. Confirm that the port and baud rate (115200 bps) are selected to match the device's UART baudrate.
2. Before starting, check the status bar in EdgeAI studio for the message "**Hardware connected**"
3. Navigate to the **Live preview** tab
4. Click **Start Preview** to begin viewing the real-time model output.

## References

- MSPM33C321A Technical Reference Manual [Link](https://www.ti.com/product/MSPM33C321A)
- [Edge AI Studio](https://dev.ti.com/edgeaistudio/)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
