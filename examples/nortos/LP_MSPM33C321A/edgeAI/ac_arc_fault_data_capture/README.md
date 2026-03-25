## Example Summary

This example demonstrates the process of capturing timeseries data from MSPM33 devices using CCStudio™ Edge AI Studio. This example captures AC current and voltage waveforms using an HSADC channel 8 and channel 2 respectively and streams the data to CCStudio™ Edge AI Studio for data acquisition.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| SYSCTL |  |  |
| UART0 | PA10 | TX Pin (Back-channel) |
| UART0 | PA11 | RX Pin (Back-channel) |
| GPIO | PC26 | LED RED |
| GPIO | PA2 | LED BLUE |
| GPIO | PC27 | LED GREEN |
| HSADC | ADC0 CH10 | Current Sensor Input |
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

1. Connect the LP_MSPM33C321A LaunchPad to your PC via USB.
2. Build and flash the example using CCS.
3. Configure the serial port in CCStudio™ Edge AI Studio. Confirm that the port and baud rate (115200 bps) are selected to match the device’s UART baudrate.
4. Before capturing, check the status bar in CCStudio™ Edge AI Studio for the message "**Hardware connected**"
5. Navigate to the **Capture** tab
    1. Specify the number of samples to capture and a label for the dataset.
    2. Click **Start Capture** to start data acquisition.

## References

- [AFCI ModelZoo Example](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo/examples/ac_arc_fault)
- MSPM33C321A Technical Reference Manual [Link](https://www.ti.com/product/MSPM33C321A)
- [Edge AI Studio](https://dev.ti.com/edgeaistudio/)
