## Example Summary

This example demonstrates a motor fault detection application using an AI model on MSPM33C321A microcontroller. The system collects vibration data from an ADXL355 accelerometer, extracts frequency-domain features using FFT, and performs inference using a pre-trained neural network model. The detected fault class is indicated by toggling onboard LEDs.

This application also supports live preview using CCStudio™ Edge AI Studio. The device transfers the inferred class and the class outputs to a host(the connected PC) over the Device Agent Protocol (DAP) via UART/USB. The live results are then previewed on the CCStudio™ Edge AI Studio.

In this example the inference happens on CPU.

The class to LED color mapping is shown below:
   - **Blue**: Class 0 (Blade Damage)
   - **Red**: Class 1 (Blade Imbalance)
   - **Green**: Class 2 (Blade Obstruction)
   - **White**: Class 3 (Normal)

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| SYSCTL |  |  |
| SPI | UC2 | ADXL355 Accelerometer |
| UART0 | PA10 | TX Pin (Back-channel) |
| UART0 | PA11 | RX Pin (Back-channel) |
| GPIO | PC26 | LED RED |
| GPIO | PA2 | LED BLUE |
| GPIO | PC27 | LED GREEN |
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

## Hardware Requirements

1. LP-MSPM33C321A LaunchPad
2. ADXL355 accelerometer (sensor)
3. Host PC with a USB/serial connection to the board (or USB DFU supporting the live DAP bridge)
3. DC Brushless Fan (BLDC) for motor fault testing [Link](https://www.digikey.in/en/products/detail/sunon-fans/EF80251S1-1000U-A99/6198727)

## Example Usage

1. **Hardware Setup**
   - Connect ADXL355 SPI lines to SPI0 (PA8 SCLK, PA9 MOSI, PB5 MISO, PA0 CS).
   - Connect ADXL355 DRDY (data-ready) to PA1 (GPIO interrupt).
   - Ensure LEDs are connected to the appropriate GPIO pins for class indication.
   - Ensure that the digital ground and power from the microcontroller is connected to respective pins on the sensor.
   - Stick the sensor to the top of the motor.

2. **Operation**
   - On startup, the application initializes the sensor and peripherals.
   - The ADXL355 accelerometer collects vibration data from the motor.
   - When new data is ready, an interrupt triggers data acquisition.
   - The application performs feature extraction using FFT and bins the frequency magnitudes.
   - Extracted features are fed to the AI model for inference.
   - The output class (fault type) is determined and stored in a circular buffer.
   - The LED corresponding to the most frequently detected class in the buffer is toggled:
     - **Blue**: Class 0 (Blade Damage)
     - **Red**: Class 1 (Blade Imbalance)
     - **Green**: Class 2 (Blade Obstruction)
     - **White**: Class 3 (Normal)
   - Live Preview:
      - Open CCStudio™ Edge AI Studio.
      - Open the motor fault example.
      - Click on the Live Preview tab.
      - Make sure the application is running on the device and click on connect to device.
      - Click on start preview to preview the predicted class and the output values of each class on the GUI.

3. **Running the Example**
   - Compile, load, and run the application on your MSPM33 device.
   - The system will continuously monitor motor vibration and indicate detected fault classes via LEDs.
   - CCStudio™ Edge AI Studio will display the predicted class and the output values of each class.
   - Make sure that the correct baudrate (115200) and COM ports are selected before connecting to the device.

## Software Details

- **Feature Extraction**: Uses ARM CMSIS-DSP library for real FFT and magnitude calculation.
- **AI Inference**: Runs a pre-trained model using TVM runtime.
- **Interrupt Handling**: Efficient data collection via GPIO interrupts.
- **LED Indication**: Fault class is indicated by toggling onboard LEDs.
- **AI model used**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)
- Samples are packed as 16-bit signed integers (high byte first) and streamed in DAP frames.
- UART settings: 115200 baud.

## Notes

- Ensure the ADXL355 sensor is properly powered and connected.
- Adjust LED and GPIO pin assignments as needed for your hardware.

## References

- MSPM33C321A Technical Reference Manual [Link](https://www.ti.com/product/MSPM33C321A)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
- [Edge AI Studio](https://dev.ti.com/edgeaistudio/)
- ADXL355 manufacturer page [link](https://www.analog.com/en/index.html)
