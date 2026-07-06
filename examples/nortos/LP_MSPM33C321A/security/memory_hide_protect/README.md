## Example Summary

This example demonstrates the Hide Protection (HDP) feature on the MSPM33C321A microcontroller. HDP provides hardware-enforced protection for sensitive flash memory regions, preventing unauthorized access to protected code and data. The example shows how to configure HDP, access protected memory with proper permissions, and demonstrates the security fault that occurs when attempting unauthorized access.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA0 | Standard Output (GREEN LED - SECURE_LED) |
| GPIOA | PA26 | Standard Output (RED LED - NONSECURE_LED) |
| GPIOA | PA27 | Standard Output (BLUE LED - TRANSITION_LED) |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock (SWCLK) |
| DEBUGSS | PA19 | Debug Data In/Out (SWDIO) |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA0 | GPIOA | PA0 | J3_10 | N/A |
| PA26 | GPIOA | PA26 | J4_37 | N/A |
| PA27 | GPIOA | PA27 | J4_38 | N/A |
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

### Overview
This example demonstrates the Hide Protection (HDP) security feature by:
1. Configuring a protected flash region (sector 30: 0x1000F000 - 0x1000F7FF)
2. Placing sensitive data and code in the protected region
3. Demonstrating successful access when HDP access is enabled
4. Demonstrating security fault when attempting unauthorized access

### Protected Memory Region
The example protects a 2KB flash sector containing:
- `sensitive_key[]` - Array of sensitive data values
- `sensitive_algorithm()` - Protected function that processes data

### LED Behavior

The example uses three LEDs to indicate different states:

**GREEN LED (PA0):**
- Blinks once during HDP configuration to confirm setup
- Turns on during Test 1 (authorized access)
- Blinks rapidly 5 times when data is successfully read from protected region

**RED LED (PA26):**
- Turns on during Test 2 when HDP access is disabled
- Indicates that protection is active before attempting unauthorized access

**ALL THREE LEDs (GREEN + RED + BLUE):**
- Turn on simultaneously when NMI fault occurs
- Indicates that HDP protection successfully blocked unauthorized access
- System enters infinite loop to preserve fault state for debugging

### Execution Sequence

1. **Initialization:**
   - System initializes peripherals and GPIO
   - HDP is configured for flash sector 30
   - GREEN LED blinks once to confirm HDP setup

2. **Test 1 - Authorized Access (GREEN LED):**
   - HDP access is enabled via `DL_GSC_setHDPAccess(GSC, true, 0)`
   - Code reads from `sensitive_key[0]` - succeeds
   - Code calls `sensitive_algorithm()` - succeeds
   - GREEN LED blinks rapidly 5 times to indicate success

3. **Test 2 - Protection Demonstration (RED LED → ALL LEDs):**
   - HDP access is disabled via `DL_GSC_setHDPAccess(GSC, false, 0)`
   - RED LED turns on to indicate protection is active
   - Code attempts to read from `sensitive_key[0]`
   - **NMI fault is triggered** by hardware protection
   - `NMI_Handler()` catches the fault
   - All three LEDs turn on to indicate fault occurred
   - System enters infinite loop in fault handler

### Expected Behavior

When running the example, you should observe:
1. GREEN LED blinks once (HDP configured)
2. Brief delay
3. GREEN LED blinks rapidly 10 times (authorized access successful)
4. Brief delay
5. RED LED turns on (protection active)
6. Brief delay
7. **All three LEDs turn on and stay on** (NMI fault caught, HDP protection working)

The system will remain in the fault handler with all LEDs on, demonstrating that HDP successfully prevented unauthorized access to the protected memory region.

### EAM (Error Aggregator Module) Verification

The NMI fault handler includes verification using the Error Aggregator Module (EAM) to confirm that the fault was caused by HDP protection:

1. **EAM Security Error Flag Check**: The handler reads `EAM_SECURITY_ERR_FLAG` and verifies that the HDP bit (`DL_EAM_SECURITY_ERROR_HDP`) is set, confirming this is an HDP-related security violation.

2. **EAM Security Error Address Check**: The handler reads `EAM_SECURITY_ERR_ADDR` and verifies it matches the address of `sensitive_key[0]` (0x1000F000), confirming the fault occurred at the expected protected memory location.

3. **Error Flag Clearing**: After verification, the handler clears the EAM security error flag using `DL_EAM_clearSecurityErrorFlag()` to acknowledge the error.

This dual verification (error flag + error address) provides robust confirmation that the NMI was triggered specifically by an HDP protection violation at the protected region, not by some other fault condition.

### Security Implications

This example demonstrates that:
- HDP provides hardware-enforced protection that cannot be bypassed in software
- Attempting to access protected memory without permission triggers an immediate fault
- The fault handler can detect and respond to security violations
- Protected code and data remain hidden even from privileged code when HDP access is disabled

### Use Cases

HDP is ideal for protecting:
- Cryptographic keys and sensitive credentials
- Proprietary algorithms and intellectual property
- Security-critical code that should not be accessible during normal operation
- Firmware update verification code
- Device-specific calibration data

### Debugging Notes

- The NMI fault is intentional and demonstrates HDP protection working correctly
- All three LEDs on indicates successful demonstration of HDP security
- The EAM registers can be inspected in the debugger to verify HDP fault details:
  - `EAM->EAM_REGS.SECURITY_ERR_FLAG` should show HDP bit set (0x004)
  - `EAM->EAM_REGS.SECURITY_ERR_ADDR` should show 0x1000F000 (sensitive_key address)
- To modify the example, adjust the `test_hdp_protection_active()` function
- The protected region address and size can be changed in `configure_hdp()`

### Important: Factory Reset Required

**After executing this application, the HDP protection for the configured region will remain active.** Before loading the next application, it is advisable to perform a factory reset using the **MSPM33_Mailbox_MassErase_Auto** command via the DSSM GEL script in Code Composer Studio (CCS). This ensures that the HDP configuration is cleared and the device is returned to a clean state for subsequent programming.

To perform a factory reset in CCS:
1. Open the **Scripts** menu
2. Navigate to **MSPM33C321A_Commands** scripts
3. Execute **MSPM33_Mailbox_MassErase_Auto**

This step is necessary because HDP protection persists across resets and can prevent normal flash programming operations if not cleared.
