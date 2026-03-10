## Example Summary

This example demonstrates how to use the I2S peripheral in controller mode with DMA to transmit audio data to the TI3254 audio codec on the CC3200 Audio Booster. The example generates a sine wave tone that is continuously played through the headphone output of the audio codec.

The DMA is configured to automatically transfer audio data from a predefined sine wave buffer to the I2S transmit register without CPU intervention, creating a continuous audio output.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output (Red LED - FAIL_LED) |
| GPIOC | PC27 | Standard Output (Green LED - PASS_LED, toggles on DMA transfer completion) |
| I2C | PA0 | I2C Serial Data line (SDA) for audio codec configuration |
| I2C | PA1 | I2C Serial Clock line (SCL) for audio codec configuration |
| I2S1 | PB21 | I2S1_AD1 (Data line) |
| I2S1 | PB22 | I2S1_WCLK (Word Clock) |
| I2S1 | PB23 | I2S1_BCLK (Bit Clock) |
| I2S1 | PB24 | I2S1_MCLK (Master Clock) |
| DMA | - | Channel 0 used for I2S data transfer |
| SYSCTL | - | - |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA0 | I2C | SDA | J5_50 | <ul><li>Connect to CC3200 Audio Booster SDA pin</ul> |
| PA1 | I2C | SCL | J5_49 | <ul><li>Connect to CC3200 Audio Booster SCL pin</ul> |
| PB21 | I2S1 | AD1 | J8_49 | <ul><li>Connect to CC3200 Audio Booster DIN pin</ul> |
| PB22 | I2S1 | WCLK | J8_50 | <ul><li>Connect to CC3200 Audio Booster WCLK pin</ul> |
| PB23 | I2S1 | BCLK | J8_51 | <ul><li>Connect to CC3200 Audio Booster BCLK pin</ul> |
| PB24 | I2S1 | MCLK | J8_52 | <ul><li>Connect to CC3200 Audio Booster MCLK pin</ul> |
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
1. Connect the MSPM33C321A LaunchPad to the CC3200 Audio Booster as described in the pin assignments
2. Connect headphones or speakers to the audio output jack on the CC3200 Audio Booster
3. Compile, load, and run the example
4. A 440 Hz sine tone will be played through the headphones/speakers

The example enters a low-power wait-for-interrupt state while audio continues to play through the DMA and I2S peripherals.
