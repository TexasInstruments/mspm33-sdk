## Example Summary

This is the non-secure application component of the Level 2 (L2) debug unlock demonstration. This application runs after the secure application has configured the security settings and transitioned control to the non-secure context. The non-secure application enters a while loop where it remains stuck, demonstrating the locked debug state.

The application is designed to test the debug unlock procedure. After the secure application transitions to this non-secure code, the application will be stuck in a while loop (waiting for variable `ii` to become non-zero). At this point, the user can send the unlock debug NS command via a GEL script, perform a system reset (NRST), and the BOOTCODE will unlock the debug access for the non-secure context only. The secure debug will remain locked.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock (SWCLK) |
| DEBUGSS | PA19 | Debug Data In/Out (SWDIO) |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
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

This non-secure application must be used in conjunction with the secure application (debug_unlock_L2_secure).

### Compile and Load Instructions

**IMPORTANT:** The flash programming sequence is critical for proper operation:

1. **First, flash the secure application (debug_unlock_L2_secure):**
   - Compile the secure application
   - Flash it to the device with flash settings: **"Erase MAIN and NONMAIN memory"**
   - This is required because the secure application contains both MAIN and NONMAIN flash contents

2. **Then, flash this non-secure application (debug_unlock_L2_nonsecure):**
   - Compile the non-secure application
   - Flash it to the device with flash settings: **"Erase MAIN memory only"**
   - This preserves the secure flash and NONMAIN flash contents from the secure application
   - **IMPORTANT for CCS >= 20.4.* users:* When loading the non-secure application, users must disable the auto run to main setting in the debugger properties. Navigate to **Run → Debugger Properties → Debugger Options** and ensure under "Auto Run Options", "On a program load or restart" is **disabled**.

### Running the Example

1. After both applications are properly loaded following the flash sequence above, run the application.

2. Run the application. The secure application will execute first, configure the security settings, and then transition to this non-secure application.

3. This non-secure application will enter a while loop and remain stuck (waiting for variable `ii` to become non-zero). This demonstrates the locked debug state.

4. At this point, the non-secure debug is locked and cannot be accessed through the debugger.

5. To unlock the non-secure debug:
   - Load and execute the GEL script that sends the unlock debug NS command with the correct password (configured in the secure application's SysConfig)
   - Perform a system reset (NRST)
   - The BOOTCODE will verify the password and unlock non-secure debug access

6. After the unlock procedure, you can now debug this non-secure application context. The secure debug will remain locked, and only the non-secure context is accessible for debugging.
