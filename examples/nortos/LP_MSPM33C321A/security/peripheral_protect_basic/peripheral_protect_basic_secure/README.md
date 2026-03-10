# peripheral_protect_basic_secure

## Example Summary

This example demonstrates **basic peripheral protection** using the GSC (Global Security Controller) Peripheral Protection Controller (PPC) on the MSPM33C321A microcontroller with TrustZone. It shows how to configure peripherals with different security attributes and demonstrates what happens when non-secure code attempts to access secure peripherals.

## Peripherals & Pin Assignments

### Secure World Peripherals

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PC27 | GREEN LED (Secure GPIO) |
| TIMG4_0 | - | Timer (Secure) |
| GPIOC | PC17 | Button (Interrupt transferred to Non-Secure) |

### Security Configuration

| Peripheral | Security Attribute | Access Control |
| --- | --- | --- |
| **GPIOA** | Secure | Only secure code can access |
| **GPIOB** | Non-Secure | Both secure and non-secure code can access |
| **TIMG4_0** | Secure | Only secure code can access |

## Security Features Demonstrated

1. **Peripheral Protection Controller (PPC) Configuration**
   - Configure GPIOA as secure-only peripheral
   - Configure GPIOB as non-secure peripheral
   - Configure Timer (TIMG4_0) as secure peripheral

2. **Security Attribution Unit (SAU) Setup**
   - Define secure and non-secure memory regions
   - Configure Non-Secure Callable (NSC) region for secure services

3. **Secure/Non-Secure Partitioning**
   - Flash memory partitioning
   - SRAM partitioning
   - Peripheral access control

4. **Violation Detection**
   - NMI handler catches security violations
   - Visual feedback when non-secure code attempts to access secure peripherals

5. **Non-Secure Callable (NSC) Functions**
   - `secure_gpio_control()` - Allows non-secure code to control secure GPIO through a secure gateway
   - `get_peripheral_security_status()` - Query peripheral security configuration

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

1. First, load the Non-Secure application. Refer to the README under peripheral_protect_basic_nonsecure for detailed loading instructions.
2. After the Non-Secure application is loaded, compile this Secure application.
3. **Optional for CCS >= 20.4.* users:** Before loading the Secure application, you may re-enable the auto run to main setting. Navigate to **Run → Debugger Properties → Debugger Options** and under "Auto Run Options", re-enable "On a program load or restart".
4. Configure Flash settings: Navigate to **Run → Debugger Properties → MSPM33 Flash Settings** and set Erase Configuration to "Do not erase Flash memory". Also enable "Reset target after program load" under Reset Configuration.
5. Load the Secure application from your workspace or external folder.
6. Begin debugging the application.

Debug symbols may need to be loaded manually for full debugging capability across secure and non-secure boundaries. Consult CCS documentation for symbol loading procedures.

> **Note:** Ensure you do **NOT** load the Secure OR Non-Secure application(s) without making the appropriate changes in Flash Settings.

> **Note:** When the Core is in a Non-Secure state, it is **not** possible to load anything into Secure memory regions. Hence, if the application is stuck (un)intentionally in the Non-Secure world, one cannot load anything into Secure regions.
Reload your Non-Secure application, and _then_ the Secure application, while ensuring that the correct MSPM33 Flash settings are being used.

### Overview

This example runs in two phases:

**Phase 1: Secure World Initialization**
1. Secure code initializes and configures peripheral security attributes
2. GREEN LED blinks 3 times, then is cleared to demonstrate secure GPIO access
3. Memory and peripheral security is configured
4. Control transfers to non-secure world

**Phase 2: Non-Secure World Execution**
1. Non-secure code accesses non-secure GPIO (GPIOB) - RED LED blinks 3 times, then stays on
2. User presses button to trigger security violation test
3. RED LED is cleared before the test
4. Non-secure code attempts to access secure GPIO (GPIOA) - **This causes a fault**
5. NMI handler catches the violation and turns on ALL LEDs (GREEN, RED, and BLUE)

### LED Behavior

| LED Color | Pin | Meaning |
| --- | --- | --- |
| **GREEN** | PC27 | Secure GPIO (GPIOA) access successful (blinks 3x in secure world, then cleared) |
| **RED** | PC26 | Non-secure GPIO (GPIOB) access successful (blinks 3x in non-secure world, then stays on, cleared before Test 2) |
| **BLUE** | PA2 | Would turn on if protection fails (should never happen) |
| **ALL LEDs ON** | GREEN + RED + BLUE | **Security violation detected!** |

### Execution Sequence

1. **Power On**: System starts in secure world
2. **GREEN LED blinks 3x, then is cleared**: Secure code successfully accesses secure GPIO
3. **Transfer to Non-Secure World**: Control passes to non-secure application
4. **RED LED blinks 3x, then stays on**: Non-secure code successfully accesses non-secure GPIO
5. **Press Button**: Triggers security violation test
6. **RED LED cleared**: Preparing for violation test
7. **ALL LEDs ON (GREEN + RED + BLUE)**: Non-secure code attempted to access secure GPIO → **Fault!**

### Expected Behavior

**Secure code accessing GPIOA (secure)** → Success (GREEN LED)
**Non-secure code accessing GPIOB (non-secure)** → Success (RED LED)
**Non-secure code accessing GPIOA (secure)** → **FAULT** (ALL LEDs)

## Security Implications

### What This Demonstrates

1. **Peripheral Isolation**: Secure peripherals cannot be accessed by non-secure code
2. **Fault Detection**: Security violations are caught and handled
3. **Controlled Access**: Non-secure code can request secure services via NSC functions
4. **Defense in Depth**: Multiple layers of security (SAU, PPC, fault handlers)
