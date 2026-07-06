# TIDA-010997 Boosterpack Multi-Sensor Data Capture

## Example Summary

This example demonstrates multi-sensor data capture for TIDA-010997 reference design using CCStudio™ Edge AI Studio. The firmware acquires data from various sensors including PIR motion sensor (via ADC), temperature & humidity (HDC3020), pressure (BMP384), ambient light (OPT4001), digital microphone (ICS43434), IMU (BMI270), and hall effect sensor (TMAG5170). Data is streamed to a host PC over the Device Agent Protocol (DAP) via UART for data acquisition, model training, verification, or analysis.

The example supports multiple sensor types:
- **PIR Motion Sensor**: Analog signal captured via ADC at 31.25 Hz sampling rate
- **Temperature & Humidity**: I2C-based HDC3020 sensor
- **Pressure**: I2C-based BMP384 barometric pressure sensor
- **Ambient Light**: I2C-based OPT4001 optical sensor
- **Digital Microphone**: I2S-based ICS43434 microphone
- **IMU**: SPI-based BMI270 6-axis accelerometer and gyroscope
- **Hall Effect Sensor**: SPI-based TMAG5170 3D linear Hall effect sensor

## Hardware Requirements

- LP-MSPM33C321A development board
- TIDA-010997 boosterpack
- USB cable for UART connection to host

## Example Usage

### Hardware Setup
1. Connect the TIDA-010997 BoosterPack to the LP-MSPM33C321A LaunchPad
2. Ensure all sensors are properly seated and connections are secure
3. Connect J7 1:2 on boosterpack
4. Configure J8,J9,J10 to use D-MIC using jumpers

### Operation
1. **Build and Flash**
   - Build the firmware using the provided build system
   - Flash the firmware to the MSPM33C321A LaunchPad
   - The device initializes all sensors (I2C, SPI, I2S, ADC) and UART/DAP communication

2. **CCStudio™ Edge AI Studio Connection**
   - Launch CCStudio™ Edge AI Studio on your host PC
   - Configure the serial port with baud rate **115200 bps**
   - Verify the status bar displays "**Hardware connected**"

3. **Data Capture**
   - Navigate to the **Capture** tab in CCStudio™ Edge AI Studio
   - Select the desired sensor from the available options:
     - PIR Motion (analog ADC data)
     - Temperature (HDC3020)
     - Humidity (HDC3020)
     - Pressure (BMP384)
     - Ambient Light (OPT4001)
     - Accelerometer/Gyroscope (BMI270)
     - Hall Effect (TMAG5170)
     - Digital Microphone (ICS43434)
   - Specify the number of samples to capture
   - Click **Start Capture** to begin data acquisition
   - Data is streamed over DAP protocol and saved in CSV format

4. **Data Flow**
   - The firmware waits for host commands over DAP
   - When streaming is active, sensor data is read using the appropriate protocol (I2C/SPI/I2S/ADC)
   - Data is framed with DAP headers (start byte, channel, payload length) and transmitted via UART
   - The host receives and logs the data for training, analysis, or visualization

## Software Details

- **Communication Protocols**:
  - UART: 115200 baud for DAP communication
  - I2C: For HDC3020, BMP384, and OPT4001 sensors
  - SPI: For BMI270 and TMAG5170 sensors
  - I2S: For ICS43434 sensor

## References

- [MSPM33C321A Datasheet](https://www.ti.com/product/MSPM33C321A)
- [HDC3020 Temperature/Humidity Sensor](https://www.ti.com/product/HDC3020)
- [OPT4001 Ambient Light Sensor](https://www.ti.com/product/OPT4001)
- [EdgeAI Studio](https://www.ti.com/tool/EDGE-AI-STUDIO)
- [TIDA-010997 Reference Design](https://www.ti.com/tool/TIDA-010997)
- [LP-MSPM33C321A Evaluation Board](https://www.ti.com/tool/LP-MSPM33C321A)

## License

Copyright (c) 2026, Texas Instruments Incorporated. All rights reserved.

This software is provided under the BSD 3-Clause License. Refer to LICENSE.txt for details.
