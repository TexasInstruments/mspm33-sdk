## Example Summary

This example demonstrates how to compensate for temperature-induced frequency drift in the RTC_A's LFXT (Low-Frequency Crystal) oscillator. The application uses the internal temperature sensor with the HSADC (High-Speed Analog-to-Digital Converter) to measure ambient temperature, calculates the expected frequency drift, and applies appropriate compensation to maintain accurate timekeeping across varying temperatures.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| SYSCTL | PA3 | LFXIN |
| SYSCTL | PA4 | LFXOUT |
| RTC_A | PA9 | RTC_A Output |
| HSADC | PB23 | Internal Temperature Sensor |
| VREF | PA23 | VREF+ |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA3 | SYSCTL | LFXIN | N/A | <ul><li>PA3 is used for LFXT functionality</ul> |
| PA4 | SYSCTL | LFXOUT | N/A | <ul><li>PA4 is used for LFXT functionality</ul> |
| PA9 | RTC_A | RTC_A OUT | N/A | <ul><li>PA9 outputs the RTC_A clock signal</ul> |
| PA23 | VREF | VREF+ | N/A | <ul><li>PA23 is used for VREF functionality</ul> |
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

This example demonstrates temperature compensation for the RTC_A's LFXT crystal oscillator:

1. The HSADC is configured to sample the internal temperature sensor
2. The temperature is calculated using a linear correction formula
3. The frequency drift of the crystal is calculated using the parabolic temperature coefficient formula:
   - df/f0 (ppm) = B × (T - T0)²
   - Where:
     - B = Parabolic coefficient (-0.04 ppm/°C² for FC-135 crystal)
     - T = Ambient temperature (°C)
     - T0 = Turnover point (25°C for FC-135 crystal)
4. The calculated drift is applied as compensation to the RTC_A

Compile, load and run the example.

To observe the effect of temperature compensation, you can:
1. Connect an oscilloscope to the RTC_A_OUT pin (PA9) to monitor the RTC_A clock signal
2. Vary the ambient temperature (e.g., using a heat gun at a safe distance or ice pack)
3. Observe how the RTC maintains accurate timing despite temperature changes

Without temperature compensation, the RTC clock would drift as temperature changes, but with this example's compensation mechanism, the timing remains stable across a wide temperature range.

## Application Design Details

The example uses the following components:

1. **HSADC**: Configured to sample the internal temperature sensor
2. **VREF**: Provides a stable 2.5V reference for the ADC
3. **RTC_A**: Configured with the LFXT as clock source and temperature compensation enabled
4. **LFXT**: External 32.768 kHz crystal oscillator

The temperature compensation algorithm:
- Measures temperature using the internal sensor
- Calculates expected frequency drift based on crystal characteristics
- Applies compensation to the RTC_A (up to ±240 ppm)
- Continuously updates compensation as temperature changes

This example demonstrates how to maintain accurate timekeeping in applications where temperature variations could affect the crystal oscillator's frequency stability.
