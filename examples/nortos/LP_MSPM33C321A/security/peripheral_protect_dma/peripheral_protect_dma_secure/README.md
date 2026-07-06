# peripheral_protect_dma_secure

## Example Summary

This example demonstrates **DMA security configuration** using the GSC (Global Security Controller) on the MSPM33C321A microcontroller with TrustZone. It shows how to configure DMA instances with different security attributes and perform secure DMA operations without cryptographic drivers.

## Peripherals & Pin Assignments

### Secure World Peripherals

| Peripheral | Pin | Function |
| --- | --- | --- |
| DMA0 | - | Secure DMA (fill operation to secure SRAM) |
| GPIOC | PC27 | GREEN LED (Secure DMA success) |
| GPIOC | PC26 | RED LED (Non-secure DMA success - from non-secure world) |

### Security Configuration

| Peripheral | Security Attribute | Access Control |
| --- | --- | --- |
| **DMA0** | Secure (default) | Only secure code can access |
| **DMA1** | Non-Secure (default) | Both secure and non-secure code can access |
| **GPIOA** | Non-Secure | Accessible from non-secure world for LED control |
| **SYSCTL** | Non-Secure | Accessible from non-secure world |

## Security Features Demonstrated

1. **DMA Security Configuration**
   - DMA0 configured for secure operations (default on MSPM33C321A)
   - DMA1 configured for non-secure operations (default on MSPM33C321A)
   - Secure SRAM buffer for DMA0 operations
   - Non-secure SRAM buffer for DMA1 operations

2. **Secure DMA Operations**
   - DMA0 fill operation to secure SRAM
   - Software-triggered DMA transfer
   - Interrupt-driven completion detection
   - Data integrity verification

3. **Security Attribution Unit (SAU) Setup**
   - Define secure and non-secure memory regions
   - Configure SRAM security (64KB secure, 192KB non-secure)

4. **NVIC Interrupt Configuration**
   - Enable DMA0 interrupt in secure NVIC
   - Proper interrupt handler implementation

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

1. First, load the Non-Secure application. Refer to the README under peripheral_protect_dma_nonsecure for detailed loading instructions.
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

**Phase 1: Secure World Initialization and DMA Operations**
1. Initialize peripherals and configure SRAM security (via SysConfig)
2. Enable DMA0 interrupt in NVIC
3. Perform secure DMA0 fill operation
4. GREEN LED turns ON (secure DMA successful)
5. Configure system for non-secure world
6. Transfer control to non-secure application

**Phase 2: Non-Secure World DMA Operations**
1. Non-secure code initializes DMA1
2. Performs block transfer in non-secure SRAM
3. RED LED turns ON (non-secure DMA successful)

### LED Behavior

| LED Color | Pin | Meaning |
| --- | --- | --- |
| **GREEN** | PC27 | Secure DMA0 fill operation completed successfully |
| **RED** | PC26 | Non-secure DMA1 block transfer completed (from non-secure world) |
| **BOTH LEDs ON** | GREEN + RED | All operations successful! |

### Execution Sequence

1. **Power On**: System starts in secure world
2. **GREEN LED ON**: Secure DMA0 fill operation successful
   - Words filled with pattern 0xDEADBEEF
   - Data integrity verified
3. **Transfer to Non-Secure World**: Control passes to non-secure application
4. **RED LED ON**: Non-secure DMA1 block transfer successful
5. **Final State**: Both LEDs ON

### Expected Behavior

**Secure DMA0 fill to secure SRAM** → Success (GREEN LED)
**Non-secure DMA1 transfer to non-secure SRAM** → Success (RED LED from non-secure world)

## Security Implications

### What This Demonstrates

1. **DMA Isolation**: Secure DMA0 operates independently from non-secure DMA1
2. **Memory Protection**: Secure SRAM is protected from non-secure access
3. **Interrupt Security**: Separate NVIC instances for secure and non-secure interrupts

### Default DMA Security

On MSPM33C321A:
- **DMA0** is secure by default
- **DMA1** is non-secure by default

No explicit security configuration is needed unless you want to change these defaults.
