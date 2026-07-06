## Example Summary

This example demonstrates a motor fault detection application using an AI model on MSPM33C321A microcontroller. The system collects vibration data from an ADXL355 accelerometer, extracts frequency-domain features using FFT, and performs inference using a pre-trained neural network model. The detected fault class is indicated by toggling onboard LEDs.

In this example the inference happens using the M33 CPU itself and no hardware accelerator is used.

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
| GPIO |  | LED indicators |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
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
2. ADXL355 accelerometer [Link](https://www.digikey.in/en/products/detail/analog-devices-inc/EVAL-ADXL355-PMDZ/7324256)
3. DC Brushless Fan (BLDC) for motor fault testing [Link](https://www.digikey.in/en/products/detail/sunon-fans/EF80251S1-1000U-A99/6198727)

## Example Usage

1. **Hardware Setup**
   - Connect ADXL355 SPI lines to SPI0 (PA8 SCLK, PA9 MOSI, PB5 MISO, PA0 CS).
   - Connect ADXL355 DRDY (data-ready) to PA1 (GPIO interrupt).
   - Ensure LEDs are connected to the appropriate GPIO pins for class indication.
   - Ensure that the digital ground and power from the microcontroller is connected to respective pins on the sensor.
   - Mount the sensor on top of the motor.

2. **Operation**
   - On startup, the application initializes the sensor and peripherals.
   - The ADXL355 accelerometer collects vibration data from the motor.
   - When new data is ready, an interrupt triggers data acquisition.
   - The application performs feature extraction using FFT and bins the frequency magnitudes.
   - Extracted features are fed to the AI model for inference.
   - The output class (fault type) is determined and stored in a circular buffer.
   - The LED corresponding to the most frequently detected class in the buffer is toggled.

3. **Running the Example**
   - Compile, load, and run the application on your MSPM33 device.
   - The system will continuously monitor motor vibration and indicate detected fault classes via LEDs.

## Software Details

- **Data Acquisition**: Vibration data collected from ADXL355 accelerometer via SPI.
- **Feature Extraction**: Uses ARM CMSIS-DSP library for real FFT and magnitude calculation.
- **AI Inference**: Runs a pre-trained MLP model using TVM runtime.
- **Interrupt Handling**: Efficient data collection via GPIO interrupts.
- **LED Indication**: Fault class is indicated by toggling onboard LEDs.
- **AI Model**: Model details are available on [Model Zoo](https://github.com/TexasInstruments/tinyml-tensorlab/tree/r1.3/tinyml-modelzoo)

## Notes

- Ensure the ADXL355 sensor is properly powered and connected.
- Adjust LED and GPIO pin assignments as needed for your hardware.
- The model uses frequency-domain features extracted from accelerometer data.

## References

- MSPM33C321A Technical Reference Manual [Link](https://www.ti.com/product/MSPM33C321A)
- [TI Neural Network Compiler Guide](https://software-dl.ti.com/mctools/nnc/mcu/users_guide/)
- TI Model Training Guide: [tinyml-tensorlab](https://github.com/TexasInstruments/tinyml-tensorlab/tree/main)
