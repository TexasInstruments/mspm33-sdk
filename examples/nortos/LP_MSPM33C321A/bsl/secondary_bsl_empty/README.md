## Example Summary

This example demonstrates a simple secondary bootloader implementation for the
MSPM33C321A device. The bootloader validates the presence of an application at
a specified address (0x10080000) and jumps to it using the GSC VTOR mechanism.

## Peripherals & Pin Assignments

N/A - This example uses only the GSC (Global Security Controller) and SYSCTL.

## BoosterPacks, Board Resources & Jumper Settings

N/A

## Example Usage

Compile, load and run the bootloader. Then program the application
(secondary_bsl_empty_app) using "Do not erase Flash memory" setting.

**Important:** After programming both examples, press the NRST button on the
LaunchPad to reset the device. This ensures the bootloader at 0x10000000
executes first. The flash loader sets SP/PC to the last programmed image,
so without a manual reset, the application would start directly.

After reset, the bootloader validates the application and jumps to it. If no
valid application is found, the device enters an infinite loop (WFI).

## Memory Layout

| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Bootloader | 0x10000000 | 0x1007FFFF | 512KB | Bootloader code |
| Application | 0x10080000 | 0x100FFFFF | 512KB | Application code |
| SRAM | 0x30000000 | 0x3003FFFF | 256KB | Shared SRAM |

**Note:** Total flash is 1MB. Adjust the split based on your requirements.

## How It Works

1. **Bootloader Execution**: On device reset, the bootloader executes from
   address 0x10000000.

2. **Application Validation**: The bootloader performs a simple validation by
   checking if the stack pointer value (first word of the application's vector
   table) points to valid SRAM (0x30000000 - 0x3003FFFF).

3. **Jump Mechanism**: If validation passes, the bootloader:
   - Sets the GSC VTOR_S register to the application address (0x10080000)
   - Issues a SYSRST (system reset)
   - The VTOR value is retained across SYSRST
   - After reset, execution begins at the application's reset vector

4. **Error Handling**: If no valid application is found, the bootloader enters
   an infinite loop (WFI).

## Programming Instructions

### Step 1: Program the Bootloader

1. Build the bootloader project
2. In CCS Flash Settings, select **"Erase Main memory only"**
3. Flash the bootloader binary via JTAG

### Step 2: Program the Application

1. Build the application project (must be linked to start at 0x10080000)
2. In CCS Flash Settings, change to **"Do not erase Flash memory"**
3. Flash the application binary via JTAG

This ensures both binaries coexist in flash memory.

### Step 3: Reset the Device

After programming both binaries, press the **NRST button** on the LaunchPad.
This is required because the flash loader sets SP/PC to the last programmed
image (the application). A manual reset ensures execution starts from the
bootloader at 0x10000000.

### Debugging

- You can debug either the bootloader or application via JTAG
- When debugging the application, set CCS to load symbols only (no flash erase)

## Creating an Application for 0x10080000

To create an application that runs at 0x10080000:

1. **Modify the linker file**: Change the FLASH origin to 0x10080000

```
MEMORY
{
    FLASH (RX)  : origin = 0x10080000, length = 0x0007FFFF
    SRAM  (RWX) : origin = 0x30000000, length = 0x0003FFFF
}

SECTIONS
{
    .intvecs: palign(16) {} > 0x10080000
    ...
}
```

2. **Build the application** with this modified linker file

3. **Flash the application** using "Do not erase Flash memory" setting

## Key Functions

### jumpToApplication()

Sets the GSC VTOR register and issues a system reset:

```c
static void jumpToApplication(uint32_t appAddress)
{
    /* Set the application's vector table address */
    DL_GSC_setVTOR_S(GSC, appAddress);

    /* Issue system reset - VTOR is retained */
    DL_SYSCTL_resetDevice(DL_SYSCTL_RESET_SYSRST);
}
```

### validateApplication()

Simple validation checking for valid stack pointer:

```c
static bool validateApplication(uint32_t appAddress)
{
    uint32_t stackPointer = *((uint32_t *)appAddress);

    /* Check if stack pointer is within valid SRAM range */
    if ((stackPointer >= 0x30000000U) && (stackPointer <= 0x30040000U)) {
        return true;
    }
    return false;
}
```
