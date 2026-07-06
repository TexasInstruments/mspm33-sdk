# peripheral_protect_basic_nonsecure

## Example Summary

This is the **non-secure world** component of the peripheral_protect_basic example. It demonstrates how non-secure code interacts with peripherals that have different security attributes, and shows what happens when attempting to access secure peripherals from non-secure code.

## Peripherals & Pin Assignments

### Non-Secure World Peripherals

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOB | PC26 | RED LED (Non-Secure GPIO) |
| GPIOB | PA2 | BLUE LED (Status indicator) |
| GPIOC | PC17 | Button (Non-Secure Interrupt) |

### Attempted Access (Will Fault)

| Peripheral | Pin | Security | Result |
| --- | --- | --- | --- |
| GPIOA | PC27 | Secure |  **Access Violation** |

## Security Features Demonstrated

1. **Authorized Non-Secure Access**
   - Successfully access non-secure GPIO (GPIOB)
   - Control RED LED

2. **Unauthorized Access Detection**
   - Attempt to access secure GPIO (GPIOA)
   - Trigger security violation
   - System fault with visual feedback

3. **Non-Secure Callable (NSC) Services**
   - Call secure services through NSC gateway
   - `secure_gpio_control()` - Control secure GPIO safely
   - `get_peripheral_security_status()` - Query security configuration

4. **Interrupt Handling**
   - Handle button interrupt in non-secure state
   - Trigger security violation test on button press

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

In order to load both the Secure and Non-Secure programs (via CCS), follow the following steps:
1. Ensure CCS Theia is installed. Download from ti.com if needed.
2. Launch CCS Theia, and start a project less debug by launching a M33 appropriate ccxml file.
3. Configure Flash settings: Navigate to **Run → Debugger Properties → MSPM33 Flash Settings** and set Erase Configuration to "Erase MAIN memory only"
4. **IMPORTANT for CCS >= 20.4.* users:** When loading the non-secure application, users must disable the auto run to main setting in the debugger properties. Navigate to **Run → Debugger Properties → Debugger Options** and ensure under "Auto Run Options", "On a program load or restart" is **disabled**.
5. Load the Non-Secure application from your workspace or external folder.
6. Once all the above steps are completed in the mentioned order, proceed to load the Secure application. Refer to the README under peripheral_protect_basic_secure for instructions.

> **NOTE:** Ensure you do NOT load the Secure OR Non-Secure application(s) without making the appropriate changes in Flash Settings.

### Overview

The non-secure application runs after the secure world has configured security attributes and transferred control. It performs two main tests:

**Test 1: Authorized Access (Success)**
- Access non-secure GPIO (GPIOB)
- RED LED blinks 3 times, then stays on
- Demonstrates successful non-secure peripheral access

**Test 2: Unauthorized Access (Fault)**
- Wait for button press
- RED LED is cleared
- Attempt to access secure GPIO (GPIOA)
- **Security violation occurs**
- NMI handler turns on ALL LEDs (GREEN, RED, and BLUE)

### LED Behavior

| LED Color | Pin | Meaning |
| --- | --- | --- |
| **GREEN** | PC27 | Secure GPIO (blinks 3x in secure world, then cleared before transfer to non-secure) |
| **RED** | PC26 | Non-secure GPIO (blinks 3x in non-secure world, then stays on, cleared before Test 2) |
| **BLUE** | PA2 | Would turn on if protection fails (should never happen) |
| **ALL LEDs ON** | GREEN + RED + BLUE | **Security violation detected!** |

### Execution Sequence

1. **Secure World**: GREEN LED blinks 3x, then is cleared (secure GPIO access)
2. **Non-Secure World Entry**: Control transfers to non-secure code
3. **Test 1 - RED LED blinks 3x, then stays on**: Non-secure GPIO access works
4. **Wait for Button Press**: System waits for user input
5. **RED LED cleared**: Preparing for Test 2
6. **Test 2 - Attempt Secure Access**: Try to access GPIOA (secure)
7. **ALL LEDs ON (GREEN + RED + BLUE)**: Security violation caught!

### Expected Behavior

**Non-secure code accessing GPIOB (non-secure)** → Success (RED LED)
**Non-secure code accessing GPIOA (secure)** → **FAULT** (ALL LEDs)

## Security Implications

### What This Demonstrates

1. **Peripheral Isolation Works**: Non-secure code cannot access secure peripherals
2. **Fault Detection**: Security violations are immediately caught
3. **Controlled Access**: Non-secure code must use NSC functions to access secure resources
4. **System Protection**: Unauthorized access doesn't compromise secure world

### Why This Matters

In a real-world application:
- **Secure peripherals** might include cryptographic accelerators, secure storage, or safety-critical sensors
- **Non-secure code** might be third-party libraries, application code, or less-trusted components
- **Peripheral protection** ensures that even if non-secure code is compromised, secure resources remain protected
