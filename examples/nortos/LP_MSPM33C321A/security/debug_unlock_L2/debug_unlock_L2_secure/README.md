## Example Summary

This example demonstrates the Level 2 (L2) debug unlock feature for non-secure context on MSPM33 devices. The secure application configures the Security Attribution Unit (SAU), SRAM and Flash security settings, and then transitions to the non-secure application. The non-secure application enters a while loop where it remains stuck, allowing the user to test the debug unlock procedure using a GEL script.

After sending the unlock debug NS command via the GEL file and performing a system reset (NRST), the BOOTCODE will unlock the debug access for the non-secure context, enabling debugging of the non-secure application. Note that secure debug will remain locked and only non-secure debug will be unlocked.

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

## Non-Main Configuration

The debug unlock password for non-secure context is configured in the NONMAIN section of the SysConfig file (`debug_unlock_L2_s.syscfg`). The password consists of eight 32-bit words (dbgnslockPW0 through dbgnslockPW7) that must be provided via the GEL script to unlock non-secure debug access.

To view or modify the debug unlock password:
1. Open `debug_unlock_L2_s.syscfg` in SysConfig
2. Navigate to the NONMAIN module
3. The debug unlock password fields (dbgnslockPW0-dbgnslockPW7) are configured with the unlock password values

**Important:** The secure debug will remain locked even after unlocking non-secure debug. Only the non-secure context debug access will be enabled through this procedure.

## Example Usage

### Compile and Load Instructions

**IMPORTANT:** The flash programming sequence is critical for proper operation:

1. **First, flash the secure application (debug_unlock_L2_secure):**
   - Compile the secure application
   - Flash it to the device with flash settings: **"Erase MAIN and NONMAIN memory"**
   - This is required because the secure application contains both MAIN and NONMAIN flash contents

2. **Then, flash the non-secure application (debug_unlock_L2_nonsecure):**
   - Compile the non-secure application
   - Flash it to the device with flash settings: **"Erase MAIN memory only"**
   - This preserves the secure flash and NONMAIN flash contents from the secure application

### Running the Example

1. After both applications are properly loaded following the flash sequence above, run the application.

2. Run the application. The secure application will execute first, configure the security settings, and then transition to the non-secure application.

3. The non-secure application will enter a while loop and remain stuck (waiting for variable `ii` to become non-zero).

4. At this point, the non-secure debug is locked and cannot be accessed through the debugger.

5. To unlock the non-secure debug:
   - Load and execute the GEL script that sends the unlock debug NS command with the correct password
   - Perform a system reset (NRST)
   - The BOOTCODE will verify the password and unlock non-secure debug access

6. After the unlock procedure, you can now debug the non-secure application context. The secure debug will remain locked.
