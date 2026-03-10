# peripheral_protect_dma_nonsecure

## Example Summary

This is the **non-secure world** component of the peripheral_protect_dma example. It demonstrates how non-secure code performs DMA operations using DMA1 (non-secure DMA instance) to transfer data within non-secure SRAM, complementing the secure DMA operations performed in the secure world.

## Peripherals & Pin Assignments

### Non-Secure World Peripherals

| Peripheral | Pin | Function |
| --- | --- | --- |
| DMA1 | - | Non-Secure DMA (block transfer in non-secure SRAM) |
| GPIOC | PC26 | RED LED (Non-Secure DMA success) |

### Memory Configuration

| Memory Region | Security | Size | Usage |
| --- | --- | --- | --- |
| SRAM Pages 0-3 | Secure | 64KB | Secure world data (DMA0 operations) |
| SRAM Pages 4-15 | Non-Secure | 192KB | Non-secure world data (DMA1 operations) |

## Security Features Demonstrated

1. **Non-Secure DMA Operations**
   - DMA1 block transfer within non-secure SRAM
   - Software-triggered DMA transfer
   - Interrupt-driven completion detection
   - Data integrity verification

2. **NVIC Interrupt Configuration**
   - Enable DMA1 interrupt in non-secure NVIC
   - Proper interrupt handler implementation

3. **Memory Isolation**
   - Non-secure buffers in non-secure SRAM
   - Cannot access secure SRAM regions

4. **SYSCTL Access**
   - Non-secure access to SYSCTL (configured by secure world)
   - Call `DL_SYSCTL_disableSleepOnExit()`

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

In order to load both the Secure and Non-Secure programs (via CCS), follow the following steps:
1. Ensure CCS Theia is installed. Download from ti.com if needed.
2. Launch CCS Theia, and start a project less debug by launching a M33 appropriate ccxml file.
3. Configure Flash settings: Navigate to **Run → Debugger Properties → MSPM33 Flash Settings** and set Erase Configuration to "Erase MAIN memory only"
4. **IMPORTANT for CCS >= 20.4.* users:** When loading the non-secure application, users must disable the auto run to main setting in the debugger properties. Navigate to **Run → Debugger Properties → Debugger Options** and ensure under "Auto Run Options", "On a program load or restart" is **disabled**.
5. Load the Non-Secure application from your workspace or external folder.
6. Once all the above steps are completed in the mentioned order, proceed to load the Secure application. Refer to the README under peripheral_protect_dma_secure for instructions.

> **NOTE:** Ensure you do NOT load the Secure OR Non-Secure application(s) without making the appropriate changes in Flash Settings.

### Overview

The non-secure application runs after the secure world has:
1. Configured SRAM security attributes
2. Performed secure DMA0 operations
3. Configured system for non-secure world
4. Transferred control to non-secure code

**Non-Secure World Execution:**
1. Initialize non-secure peripherals
2. Enable DMA1 interrupt in non-secure NVIC
3. Initialize source buffer with test pattern values
4. Configure DMA1 for block transfer
5. Start DMA1 transfer (source → destination in non-secure SRAM)
6. Wait for DMA completion interrupt
7. Verify data integrity
8. RED LED turns ON (non-secure DMA successful)

### LED Behavior

| LED Color | Pin | Meaning |
| --- | --- | --- |
| **GREEN** | PC27 | Secure DMA0 operation successful (from secure world) |
| **RED** | PC26 | Non-secure DMA1 operation successful |
| **BOTH LEDs ON** | GREEN + RED | All operations successful! |

### Execution Sequence

1. **Secure World** (before non-secure entry):
   - GREEN LED ON: Secure DMA0 fill operation successful

2. **Non-Secure World Entry**: Control transfers to non-secure code

3. **Non-Secure DMA Operations**:
   - Initialize source buffer with test pattern
   - Clear destination buffer
   - Configure DMA1 Channel 0 for block transfer
   - Start DMA1 transfer
   - Wait for interrupt (DMA1_IRQHandler sets flag)
   - Verify: destination[i] == source[i] for all i
   - **RED LED ON**: Non-secure DMA1 successful

4. **Final State**: Both LEDs ON (GREEN + RED)

### Expected Behavior

**Non-secure DMA1 block transfer** → Success (RED LED)
**Data integrity verification** → All words match
**Interrupt handling** → DMA1_IRQHandler called correctly

## Security Implications

### What This Demonstrates

1. **DMA Isolation**: Non-secure DMA1 operates independently from secure DMA0
2. **Memory Protection**: Non-secure code can only access non-secure SRAM
3. **Interrupt Security**: Non-secure NVIC handles non-secure interrupts
4. **Peripheral Access**: Non-secure code can access non-secure peripherals (DMA1, GPIOA, SYSCTL)

### Memory Boundaries

**Non-Secure SRAM:**
- Base Address: 0x20010000 (Page 4)
- Size: 192KB (Pages 4-15)
- Usage: Non-secure application data, DMA1 buffers

**Secure SRAM (Cannot Access):**
- Base Address: 0x30000000 (secure alias)
- Size: 64KB (Pages 0-3)
- Usage: Secure application data, DMA0 buffers

Attempting to access secure SRAM from non-secure code would cause a security violation.
