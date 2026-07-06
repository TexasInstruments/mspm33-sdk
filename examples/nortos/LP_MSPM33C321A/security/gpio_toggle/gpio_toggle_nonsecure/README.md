## Example Summary

This example demonstrates ARM TrustZone-M non-secure world operation on the MSPM33C321A. The non-secure application is launched by the secure application, executes in non-secure memory regions, handles GPIO interrupts, and invokes secure functions via the Non-Secure Callable (NSC) region.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA2 | Standard Output (TRANSITION_LED) |
| GPIOC | PC17 | Standard with Wake Input with internal pull-up (USER_SWITCH) |
| GPIOC | PC26 | Standard Output (NONSECURE_LED) |
| GPIOC | PC27 | Standard Output (SECURE_LED) |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock (SWCLK) |
| DEBUGSS | PA19 | Debug Data In/Out (SWDIO) |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA2 | GPIOA | PA2 | N/A | N/A |
| PC17 | GPIOC | PC17 | N/A | N/A |
| PC26 | GPIOC | PC26 | N/A | N/A |
| PC27 | GPIOC | PC27 | N/A | N/A |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging</ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging</ul> |

### Device Migration Recommendations

This project was developed for the MSPM33C321A device included in the LP_MSPM33C321A LaunchPad.

### Low-Power Recommendations

TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau869).

## Example Usage

### Loading and Debugging with CCS

There are two methods to load TrustZone applications:

#### **Method 1: Load Combined Image**

> **Note:** This method currently works only for TIARMCLANG-based examples and is not supported for GCC examples.

When you build the non-secure application, the secure application is automatically built and a combined `<example_name>_full.txt` file is generated.

**Steps:**
1. Build the non-secure application
2. Load the generated `<example_name>_full.txt` file, which is present in the Debug folder of the Secure application, inside the workspace, in CCS

**Debugging:**
The combined image does not halt at main. To debug:
- Add temporary infinite loops at points where you want to halt
- Load debug symbols manually
- Continue debugging from the halt points

#### **Method 2: Load Applications Separately**

In order to load both the Secure and Non-Secure programs(via CCS), follow the following steps:
1. Ensure CCS Theia is installed. Download from ti.com if needed.
2. Launch CCS Theia, and start a project less debug by launching a M33 appropriate ccxml file.
3. Configure Flash settings: Navigate to **Run → Debugger Properties → MSPM33 Flash Settings** and set Erase Configuration to "Erase MAIN memory only". Also enable "Reset target after program load" under Reset Configuration.
4. **IMPORTANT for CCS >= 20.4.* users:** When loading the non-secure application, users must disable the auto run to main setting in the debugger properties. Navigate to **Run → Debugger Properties → Debugger Options** and ensure under "Auto Run Options", "On a program load or restart" is **disabled**.
5. After changing the Flash Settings, load the Non-Secure application from your workspace/external folder.
6. Once all the above steps are completed in the mentioned order, proceed to load the Secure application. To do so, read the README under gpio_toggle_secure.

Debug symbols may need to be loaded manually for full debugging capability across secure and non-secure boundaries. Consult CCS documentation for symbol loading procedures.

> NOTE : Ensure you do NOT load the Secure OR Non-Secure application(s) without making the appropriate changes in Flash Settings.

> **Note** : When the Core is in a Non-Secure state, it is **not** possible to load anything into Secure memory regions. Hence, if the application is stuck (un)intentionally in the Non-Secure world, one cannot load anything into Secure regions.
Reload your Non-Secure application, and _then_ the Secure application, while ensuring that the correct MSPM33 Flash settings are being used.



### Overview
This non-secure application demonstrates ARM TrustZone-M non-secure world operation by:
1. Executing in non-secure memory regions configured by the secure application
2. Handling GPIO interrupts transferred from secure to non-secure state
3. Controlling non-secure GPIO resources
4. Invoking secure functions via NSC gateway mechanism
5. Demonstrating secure/non-secure world transitions

The program begins execution in the Secure region's main function, which turns on the GREEN LED. It calls the Non-Secure Reset Handler, which in turn calls the Non-Secure main region's main function, turning on the RED LED. The program waits for an interrupt, which can be issued by pressing the PC17 button. This will call a function present in the secure region, via the Non Secure Callable(NSC) region. When inside the called secure function, the GREEN LED is on. Once the secure function executes, it returns back to the Non-Secure program, turning the RED LED on again.

The GREEN LED turning on indicates that the execution is occurring in the Secure World and the RED LED turning on indicated that the execution is occurring the Non-Secure World, since these LEDs are turned on _only_ in the associated Secure/Non-Secure region.

### Companion Project

Requires `gpio_toggle_secure` project for complete functionality:
- Secure world initialization and memory configuration
- SAU and GSC security attribute setup
- NSC gateway function implementation
- Secure to non-secure state transition
