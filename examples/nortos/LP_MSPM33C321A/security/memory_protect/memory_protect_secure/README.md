## Example Summary

This example demonstrates ARM TrustZone-M security state transition on the MSPM33C321A. The secure application configures memory protection, establishes security boundaries, and transitions execution to a Non-Secure(NS) application. The NS application jumps back to some Secure code, via the Non Secure Callable(NSC) region, after which it returns to NS.

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

1. First, load the Non-Secure application. Refer to the README under memory_protect_nonsecure for detailed loading instructions.
2. After the Non-Secure application is loaded, compile this Secure application.
3. **Optional for CCS >= 20.4.* users:** Before loading the Secure application, you may re-enable the auto run to main setting. Navigate to **Run → Debugger Properties → Debugger Options** and under "Auto Run Options", re-enable "On a program load or restart".
4. Configure Flash settings: Navigate to **Run → Debugger Properties → MSPM33 Flash Settings** and set Erase Configuration to "Do not erase Flash memory".
5. Load the Secure application from your workspace or external folder.
6. Begin debugging the application.

Debug symbols may need to be loaded manually for full debugging capability across secure and non-secure boundaries. Consult CCS documentation for symbol loading procedures.

> **Note:** Ensure you do **NOT** load the Secure OR Non-Secure application(s) without making the appropriate changes in Flash Settings.

> **Note:** When the Core is in a Non-Secure state, it is **not** possible to load anything into Secure memory regions. Hence, if the application is stuck (un)intentionally in the Non-Secure world, one cannot load anything into Secure regions.
Reload your Non-Secure application, and _then_ the Secure application, while ensuring that the correct MSPM33 Flash settings are being used.

### Overview
This secure world application demonstrates ARM TrustZone-M security features by:
1. Configuring memory partitioning between secure and non-secure regions
2. Configuring the Security Attribution Unit (SAU) for different memory regions
3. Configuring the Global Security Controller (GSC) for SRAM and Flash security attributes
4. Performing secure-world memory access tests before transitioning to non-secure execution
5. Providing Non-Secure Callable (NSC) functions for controlled access to secure resources


The program begins execution in the Secure region's main function, which initializes secure peripherals and GPIO. It then performs two secure-world memory access tests:

**Test 1:** Secure code accesses secure memory (secure_data_buffer) - demonstrates normal secure operation. The GREEN LED blinks during this test.

**Test 2:** Secure code accesses non-secure memory (non-secure SRAM) - demonstrates that secure code can safely access non-secure regions. The GREEN LED blinks during this test.

After completing these tests, the secure application configures the button interrupt (PC17) to target the non-secure state via the NVIC_ITNS register, then transitions execution to the Non-Secure world by calling the non-secure reset handler. The non-secure application continues with additional memory protection tests.

The secure application also provides NSC functions that allow the non-secure world to safely access secure resources:
- `read_secure_data()` - Allows controlled read access to secure memory
- `write_secure_data()` - Allows controlled write access to secure memory
- `get_secure_buffer_address()` - Returns secure buffer address for testing
- `get_nsc_region_address()` - Returns NSC region address for testing

A custom HardFault handler catches security violations, turning on all three LEDs (GREEN, RED, and BLUE) to indicate a fault state and preserving system state for debugging.

The GREEN LED indicates secure world execution, while all three LEDs on simultaneously indicate a security fault has occurred.

### Companion Project

Requires `memory_protect_nonsecure` project for complete functionality:
- Non-secure GPIO operations
- NSC gateway invocation
- Non-secure interrupt handling
