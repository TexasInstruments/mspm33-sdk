## Example Summary

This is a sample application designed to work with the `secondary_bsl_empty`
bootloader. The application is linked to start at address 0x10080000 and
demonstrates successful bootloader-to-application transition by blinking an LED.

## Peripherals & Pin Assignments

| Peripheral  | Pin  | Function |
| ----------- | ---- | -------- |
| GPIOA       | PA2  | USER_LED_1 (Blue) |
| GPIOC       | PC26 | USER_LED_2 (Red) |
| GPIOC       | PC27 | USER_LED_3 (Green) |

## BoosterPacks, Board Resources & Jumper Settings

N/A

## Example Usage

Compile, load and run the example after programming the bootloader
(secondary_bsl_empty). Use "Do not erase Flash memory" setting when flashing.

**Important:** After programming both examples, press the NRST button on the
LaunchPad to reset the device. This ensures the bootloader at 0x10000000
executes first. The flash loader sets SP/PC to the last programmed image,
so without a manual reset, the application would start directly bypassing
the bootloader.

After reset, the LEDs (PA2, PC26, PC27) should blink at approximately 500ms
intervals, confirming successful bootloader to application transition.

## Memory Layout

This application is linked to run at address 0x10080000 (512KB offset from flash
start). The custom linker file `ticlang/mspm33c321a_application.cmd` places:

- Vector table at 0x10080000
- Code sections in FLASH starting at 0x10080000
- Data sections in SRAM (0x30000000)

## How It Works

1. The bootloader (secondary_bsl_empty) validates this application
2. Bootloader sets GSC VTOR to 0x10080000 and issues SYSRST
3. After reset, CPU begins execution at this application's reset vector
4. Application initializes and blinks LED at ~500ms intervals

## Programming Instructions

### Prerequisites

Ensure the bootloader (secondary_bsl_empty) is already programmed at
address 0x10000000.

### Step 1: Build the Application

Build this project using CCS or the command line. The linker file will
automatically place the application at 0x10080000.

### Step 2: Flash the Application

1. In CCS Flash Settings, select **"Do not erase Flash memory"**
   - This preserves the bootloader that was previously programmed
2. Flash the application binary via JTAG

### Verification

After programming both bootloader and application:
1. Press the **NRST button** on the LaunchPad to reset the device
   - This is required because the flash loader sets SP/PC to the last programmed
     image, bypassing the bootloader
2. LEDs (PA2, PC26, PC27) should blink at approximately 500ms intervals
3. This confirms successful bootloader -> application transition

## Debugging

When debugging this application:
1. You can set breakpoints and step through code
2. The debugger will load symbols for addresses starting at 0x10080000
3. To debug the bootloader, load the bootloader project instead

## Customization

To change the application start address:

1. Modify `APPLICATION_START_ADDRESS` in the bootloader source
2. Update the linker file `mspm33c321a_application.cmd`:
   - Change FLASH origin
   - Change .intvecs placement

## Files

| File | Description |
|------|-------------|
| secondary_bsl_empty_app.c | Main application source |
| secondary_bsl_empty_app.syscfg | SysConfig configuration |
| ticlang/mspm33c321a_application.cmd | Custom linker file for 0x10080000 |
| README.md | This file |
