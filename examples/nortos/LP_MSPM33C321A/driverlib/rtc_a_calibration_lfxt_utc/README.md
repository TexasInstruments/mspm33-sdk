## Example Summary

This example demonstrates how to implement continuous UTC timestamp printing using the RTC_A PS1(1Hz) interrupt while performing both static and dynamic calibration of the RTC_A's Low-Frequency External Crystal (LFXT). The application performs a one-time static offset calibration at startup and then uses a timer to perform dynamic temperature-based calibration every 5 minutes (300 seconds) to maintain accurate timekeeping across varying temperatures and compensate for crystal drift. For drift values exceeding ±240 PPM, both calibration methods implement a rounds-based approach with 1-minute gaps between rounds.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| RTC_A | PA9 | RTC_A Output |
| HSADC | N/A | Internal Temperature Sensor (CH11) |
| VREF | PA23 | VREF+ |
| TIMER | N/A | Timer for periodic calibration |
| UART | PA10 | UART TX |
| UART | PA11 | UART RX |
| SYSCTL | PA3 | LFXIN |
| SYSCTL | PA4 | LFXOUT |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA9 | RTC_A | RTC_A OUT | N/A | <ul><li>PA9 outputs the RTC_A clock signal</ul> |
| PA23 | VREF | VREF+ | N/A | <ul><li>PA23 is used for VREF functionality</ul> |
| PA10 | UART | TX | N/A | <ul><li>PA10 is UART TX (115200 baud)</ul> |
| PA11 | UART | RX | N/A | <ul><li>PA11 is UART RX (115200 baud)</ul> |
| PA3 | SYSCTL | LFXIN | N/A | <ul><li>PA3 is used for LFXT functionality</ul> |
| PA4 | SYSCTL | LFXOUT | N/A | <ul><li>PA4 is used for LFXT functionality</ul> |
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

This example demonstrates both static offset calibration and dynamic temperature-based calibration for the RTC_A's LFXT:

1. At startup, the application prompts the user to enable/disable static offset calibration
   - If enabled, the user can input the measured RTC_A frequency for calibration
   - The application calculates the crystal drift in PPM and applies appropriate compensation
   - For drift values exceeding ±240 PPM, calibration is performed in rounds:
     - Each round applies at most ±240 PPM of calibration
     - A 1-minute gap is enforced between calibration rounds
     - This process continues until the drift is within the ±240 PPM limit

2. The user is then prompted to enable/disable dynamic temperature calibration
   - If enabled, the application will perform temperature-based calibration every 5 minutes
   - For temperature drift values exceeding ±240 PPM, the same rounds-based approach is used:
     - Each round applies at most ±240 PPM of calibration
     - A 1-minute gap is enforced between calibration rounds
     - This process continues until the drift is within the ±240 PPM limit

3. The RTC_A continuously outputs UTC timestamps via the RTC_A PS1 interrupt
   - Timestamps are printed in the format: YYYY-MM-DD DW HH:MM:SS
   - Where DW is the day of the week (0-6, where 0 is Sunday)

4. Dynamic temperature calibration (if enabled):
   - The HSADC samples the internal temperature sensor with CH11
   - Temperature is calculated using a linear correction formula
   - The frequency drift is calculated using the parabolic temperature coefficient formula:
     - df/f0 (ppm) = B × (T - T0)²
     - Where:
       - B = Parabolic coefficient (-0.04 ppm/°C² for FC-135 crystal)
       - T = Ambient temperature (°C)
       - T0 = Turnover point (25°C for FC-135 crystal)
   - The calculated drift is applied as compensation to the LFXT

### Hardware Setup
If you want to monitor the RTC_A frequency, connect an oscilloscope probe to the PA9 pin

### Sample Output

```
Do you want to perform static offset calibration? (y/n): y
Static offset calibration enabled
Reference RTC_A frequency: 512.00 Hz
Enter measured RTC_A frequency: 511.95
Crystal Drift: 16 PPM
Round 1: Applying 16 PPM calibration
Waiting for 60 seconds after the Offset Calibration is completed in 1 rounds

Do you want to enable dynamic temperature calibration? (y/n): y
Dynamic temperature calibration enabled
UTC : 2026-1-1 0 10:33:0
UTC : 2026-1-1 0 10:33:1
...
Processing dynamic calibration after timer event : 5 minutes elapsed
Temperature: 23.50°C, Drift: -9 PPM
Round 1: Applying -9 PPM temperature compensation
Waiting for 60 seconds after the temperature Calibration is completed in 1 rounds
```

