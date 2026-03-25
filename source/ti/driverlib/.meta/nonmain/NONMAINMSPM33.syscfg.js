/*
 * Copyright (c) 2023 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 *  ======== moduleTimerMSP432.js ========
 */

"use strict";

/* get Common /ti/driverlib utility functions */
let Common = system.getScript("/ti/driverlib/Common.js");

let deviceOptions = system.getScript("/ti/driverlib/nonmain/NONMAINOptions.js");


let enableOptions = [
    { name: "enabled", displayName: "Level 0 - Full Access" },
    { name: "level1", displayName: "Level 1 - Secure Protected, Non-Secure Enabled" },
    { name: "level2", displayName: "Level 2 - Both Secure and Non-Secure Protected" },
    { name: "disabled", displayName: "Disabled" }
];


let factoryResetOptions = [
    { name: "enabled", displayName: "Enabled without Password" },
    { name: "disabled", displayName: "Always Disabled" },
    { name: "enabledWithPW", displayName: "Enabled with Password" }
];

let massEraseOptions = [
    { name: "enabled", displayName: "Enabled without Password" },
    { name: "disabled", displayName: "Always Disabled" },
    { name: "enabledWithPW", displayName: "Enabled with Password" }
];

/* Device-Specific Configurables */
/* BCR Configurables */

let extendedConfigBCR = [
    {
        // .CSCexist
        name: "cscExists",
        displayName: "Enable CSC Policy",
        description: "Enable / disable the CSC policy in SYSCTL",
        default: false,
    },
    {
        // .debugHold
        name: "debugHold",
        displayName: "Debug Hold",
        description: "Controls the release of debug access until INITDONE is issued.",
        default: false,
    },
    {
        // .flashBankSwapPolicy
        name: "flashBankSwap",
        displayName: "Enable Flash Bank Swap Policy",
        description: "Enable / Disable the Flash Bank Swap Policy in SYSCTL",
        default: false,
    },
];

let extendedConfigBCRCRC = [
    {
        // NOTE: this feature is currently disabled/hidden
        // .userSecureAppHash
        // TODO: this will most likely require separate fields
        //          for each of the required values depending on
        //          the chosen addresses/size. Leaving as
        //          placeholder for now.
        // TODO: this would be replaced by a GROUP of configurables similar
        //          to the ones used for password, can create/repurpose
        //          similar function. However, this is tied to re-enabling
        //          the user app checking feature, so this is less priority.
        name: "appCRCHash",
        displayName: "Application CRC Check Application Hash",
        description: "",
        longDescription: ``,
        hidden: true,
        displayFormat: "hex",
        default: 0xFFFFFFFF,
        range: [0, 0xFFFFFFFF]
    },
];

let warningNoteM0C = "";
if (Common.isDeviceM0C()) {
    warningNoteM0C = `
**Note: This feature is not available on the following devices: MSPM0C110x, MSPS003Fx.**
    `;
}

// Used for 128-bit passwords
let sha256Note = "";
// used for 256-bit passwords
let sha256NoteAlt = "";
if (deviceOptions.SUPPORT_PW_HASH == true) {
    sha256Note = `
**Note**: Password fields for this device **require** the user to provide the SHA256
hash of the 128-bit password. Refer to the following steps on correctly converting
between 128-bit plaintext password and hash:

1. Determine the 128-bit password to use. These steps will use the following
example password:

* 0x12345678\n
* 0xCAFECAFE\n
* 0xDEADBEEF\n
* 0xCAFEBABE\n

2. Reverse the password endianness. Combine it into one string.
* 0x78563412\n
* 0xFECAFECA\n
* 0xEFBEADDE\n
* 0xBEBAFECA\n

\tCombined String: 78563412FECAFECAEFBEADDEBEBAFECA

3. Calculate the SHA256 of the string. Break the output into 8 32-bit words.
The user can use online tools like the one provided here:
[SHA256 online tool](https://emn178.github.io/online-tools/sha256.html)
**Important**: Make sure to select **Hex** as the input encoding in the tool.

\tOutput String: 2738a5682ad52550177227468dd2b55f552c34bd25560b9067b0a3f3d5c648f9

\tResult:\n
* 0x2738a568\n
* 0x2ad52550\n
* 0x17722746\n
* 0x8dd2b55f\n
* 0x552c34bd\n
* 0x25560b90\n
* 0x67b0a3f3\n
* 0xd5c648f9\n

4. Reverse endianness of the 32-bit words.

\tOriginal   -> Converted\n
* 0x2738a568 -> 0x68A53827\n
* 0x2ad52550 -> 0x5025D52A\n
* 0x17722746 -> 0x46277217\n
* 0x8dd2b55f -> 0x5FB5D28D\n
* 0x552c34bd -> 0xBD342C55\n
* 0x25560b90 -> 0x900B5625\n
* 0x67b0a3f3 -> 0xF3A3B067\n
* 0xd5c648f9 -> 0xF948C6D5\n

5. Enter the converted password into the BCR (boot configuration routine) structure
for the desired function e.g. mass erase, factory reset, or debug lock.
`;

    sha256NoteAlt = `
**Note**: This password field **requires** the user to provide the SHA256
hash of the 256-bit password.

Refer to the following steps on correctly converting
between a 256-bit plaintext password and hash for reference:

1. Determine the 256-bit password to use. These steps will use the following
example password:

* 0x12345678\n
* 0xCAFECAFE\n
* 0xDEADBEEF\n
* 0xCAFEBABE\n
* 0xABCD1234\n
* 0x5678EFAB\n
* 0x9ABC0DEF\n
* 0xFEDCBA98\n

2. Reverse the password endianness. Combine it into one string.
* 0x78563412\n
* 0xFECAFECA\n
* 0xEFBEADDE\n
* 0xBEBAFECA\n
* 0x3412CDAB\n
* 0xABEF7856\n
* 0xEF0DBC9A\n
* 0x98BADCFE\n

\tCombined String: 78563412FECAFECAEFBEADDEBEBAFECA3412CDABABEF7856EF0DBC9A98BADCFE

3. Calculate the SHA256 of the string. Break the output into 8 32-bit words.
The user can use online tools like the one provided here:
[SHA256 online tool](https://emn178.github.io/online-tools/sha256.html)
**Important**: Make sure to select **Hex** as the input encoding in the tool.

\tOutput String: 1ebee0beac223aaa2a5ee3b5f8c4f0b7ce36b3e3cc943c1a5fbac0014a02ec7d

\tResult:\n
* 0x1ebee0be\n
* 0xac223aaa\n
* 0x2a5ee3b5\n
* 0xf8c4f0b7\n
* 0xce36b3e3\n
* 0xcc943c1a\n
* 0x5fbac001\n
* 0x4a02ec7d\n

4. Reverse endianness of the 32-bit words.

\tOriginal   -> Converted\n
* 0x1ebee0be -> 0xBEE0BE1E\n
* 0xac223aaa -> 0xAA3A22AC\n
* 0x2a5ee3b5 -> 0xB5E35E2A\n
* 0xf8c4f0b7 -> 0xB7F0C4F8\n
* 0xce36b3e3 -> 0xE3B336CE\n
* 0xcc943c1a -> 0x1A3C94CC\n
* 0x5fbac001 -> 0x01C0BA5F\n
* 0x4a02ec7d -> 0x7DEC024A\n

5. Enter the converted password into the BSL (bootstrap loader) configuration structure.
`;
}


function formatAddress(addr) {
    return `0x${addr.toString(16).toUpperCase().padStart(8, '0')}`;
}

function getDisabledEnableOptions(inst) {
    /* Password authentication for any SWD commands isn't supported on M0C */
    let disabledOption = [
        {
            name: "enabledWithPW",
            displayName: "Enabled with password match",
            reason: "Not available on M0C devices"
        }
    ];

    if (Common.isDeviceM0C()) {
        return disabledOption;
    }
    else {
        return [];
    }
}

const BANK0_SECTORS = (() => {
    const options = [];
    const BANK0_BASE = 0x10000000;

    // First 32 sectors (2KB each)
    for (let i = 0; i < 32; i++) {
        const startAddr = BANK0_BASE + (i * 0x800);
        const endAddr = startAddr + 0x7FF;
        options.push({
            name: `sector${i}`,
            displayName: `Sector ${i} (0x${startAddr.toString(16).toUpperCase()}-0x${endAddr.toString(16).toUpperCase()})`,
            description: `2KB sector starting at 0x${startAddr.toString(16).toUpperCase()}`
        });
    }

    // Next 32 blocks (16KB each)
    // Calculate base address for blocks after 32 2KB sectors
    const BLOCKS_BASE = BANK0_BASE + (32 * 0x800);  // Start after 32 2KB sectors

    for (let i = 32; i < 64; i++) {
        const blockIndex = i - 32;  // Index from 0 for blocks
        const startAddr = BLOCKS_BASE + (blockIndex * 0x4000);  // 0x4000 = 16KB
        const endAddr = startAddr + 0x3FFF;
        options.push({
            name: `block${i}`,
            displayName: `Block ${i} (0x${startAddr.toString(16).toUpperCase()}-0x${endAddr.toString(16).toUpperCase()})`,
            description: `16KB block starting at 0x${startAddr.toString(16).toUpperCase()}`
        });
    }

    return options;
})();

const BANK1_SECTORS = (() => {
    const options = [];
    const BANK1_BASE = 0x10080000;

    // First 32 sectors (2KB each)
    for (let i = 0; i < 32; i++) {
        const startAddr = BANK1_BASE + (i * 0x800);  // 0x800 = 2KB
        const endAddr = startAddr + 0x7FF;
        options.push({
            name: `sector${i}`,
            displayName: `Sector ${i} (0x${startAddr.toString(16).toUpperCase()}-0x${endAddr.toString(16).toUpperCase()})`,
            description: `2KB sector starting at 0x${startAddr.toString(16).toUpperCase()}`
        });
    }

    // Next 32 blocks (16KB each)
    // Calculate base address for blocks after 32 2KB sectors
    const BLOCKS_BASE = BANK1_BASE + (32 * 0x800);  // Start after 32 2KB sectors

    for (let i = 32; i < 64; i++) {
        const blockIndex = i - 32;  // Index from 0 for blocks
        const startAddr = BLOCKS_BASE + (blockIndex * 0x4000);  // 0x4000 = 16KB
        const endAddr = startAddr + 0x3FFF;
        options.push({
            name: `block${i}`,
            displayName: `Block ${i} (0x${startAddr.toString(16).toUpperCase()}-0x${endAddr.toString(16).toUpperCase()})`,
            description: `16KB block starting at 0x${startAddr.toString(16).toUpperCase()}`
        });
    }

    return options;
})();

const DBANK_SECTORS = (() => {
    const options = [];
    const DBANK_BASE = 0x80000000;  // DBANK base address
    const SECTOR_SIZE = 0x800;      // 2KB
    const NUM_SECTORS = 16;         // Only 16 sectors in DBANK

    // 16 sectors (2KB each)
    for (let i = 0; i < NUM_SECTORS; i++) {
        const startAddr = DBANK_BASE + (i * SECTOR_SIZE);
        const endAddr = startAddr + (SECTOR_SIZE - 1);
        options.push({
            name: `sector${i}`,
            displayName: `Sector ${i} (${formatAddress(startAddr)}-${formatAddress(endAddr)})`,
            description: `2KB sector starting at ${formatAddress(startAddr)}`
        });
    }

    return options;
})();

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst) {
    let pin;

    if (inst.module) {
        let modPin = "DIO" + inst.module.modulePin.$solution.devicePinName;
        modPin = modPin.match(/P\d+\.\d+/)[0];

        pin = "\nMODPIN: " + modPin;

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += "\n" + inst.$hardware.displayName;
        }
    }

    return (pin);
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  param inst       - module instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validateNONMAIN(inst, validation) {
    /*
     * User must acknowledge NONMAIN configuration risks before proceeding with
     * configuration and programming
     */
    if (!inst.nonmainWarning) {
        validation.logError(
            "User must accept Configuration NVM (NONMAIN) configuration risks",
            inst, "nonmainWarning"
        );

        validation.logInfo(
            "Using the Configuration NVM (NONMAIN) has certain risks. Please " +
            "refer to the configuration risks fields's long description by " +
            "clicking the question mark next to the field for more details " +
            "on the risks", inst
        );
    }

    /* Validate BCR region */
    validateBCR(inst, validation);

    validateBSL(inst, validation);
}

function validateBCR(inst, validation) {
    /* Debug security port validation */
    if (!inst.swdEnable) {
        validation.logWarning(
            "When the physical debug port (SW-DP) is disabled, all of the " +
            "SWD-accessible functions (application debug, mass erase, factory " +
            "reset, and TI failure analysis) are not accessible through SWD, " +
            "regardless of their individual configuration.",
            inst, "swdEnable"
        );
    }

    /* HFXT frequency validation for MSP M33 launchpad */
    if (inst.syspllSource === "0xAA") { // 0xAA is the value for HFXT 20MHz
        validation.logWarning(
            "The launchpad comes with a 40MHz HFXT crystal. While 20MHz " +
            "operation is supported by ROM, it does not match the hardware configuration " +
            "of the launchpad. Please ensure this selection matches your hardware setup.",
            inst, "syspllSource"
        );
    }


    /* NONMAIN flash write protection validation */
    if (inst.nonMainWriteProtection === "enabled") {
        validation.logWarning(
            "This locks the configuration permanently unless BCR factory reset " +
            "is enabled with or without password.",
            inst, "nonMainWriteProtection"
        );
    }

    /* Fast boot mode and CRC check validation */
    if (inst.fastBootMode) {
        validation.logWarning(
            "If enabled, the application CRC check will be bypassed even if " +
            "the CRC check is enabled.",
            inst, "fastBootMode"
        );
    }

    /* M0C-specific BCR validation */
    if (Common.isDeviceM0C()) {
        if (inst.apEnable == "enabledWithPW") {
            validation.logError(
                "The selected option is not supported on M0C. Please reselect.",
                inst, "apEnable"
            );
        }

        if (inst.frMode == "enabledWithPW") {
            validation.logError(
                "The selected option is not supported on M0C. Please reselect.",
                inst, "frMode"
            );
        }
    }
}

function validateBSL(inst, validation) {

    validatePinOptions(inst, validation);

    validateSelectedPins(inst, validation);
}

function validatePinOptions(inst, validation) {
    // Check if UART pins can be generated
    let uartTXOptions = createUARTTXPinOptions(inst);
    let uartRXOptions = createUARTRXPinOptions(inst);

    if (uartTXOptions.length === 0) {
        validation.logError(
            `No valid UART TX pins found for peripheral ${inst.uartPeripheral}`,
            inst, "uartPeripheral"
        );
    }

    if (uartRXOptions.length === 0) {
        validation.logError(
            `No valid UART RX pins found for peripheral ${inst.uartPeripheral}`,
            inst, "uartPeripheral"
        );
    }

    // Check if I2C pins can be generated
    let i2cSCLOptions = createI2CSCLPinOptions(inst);
    let i2cSDAOptions = createI2CSDAPinOptions(inst);

    if (i2cSCLOptions.length === 0) {
        validation.logError(
            `No valid I2C SCL pins found for peripheral ${inst.i2cPeripheral}`,
            inst, "i2cPeripheral"
        );
    }

    if (i2cSDAOptions.length === 0) {
        validation.logError(
            `No valid I2C SDA pins found for peripheral ${inst.i2cPeripheral}`,
            inst, "i2cPeripheral"
        );
    }

    // Check MCAN pins if supported
    if (deviceOptions.BSL_SUPPORT_MCAN) {
        let mcanTXOptions = createMCANTXPinOptions(inst);
        let mcanRXOptions = createMCANRXPinOptions(inst);

        if (mcanTXOptions.length === 0) {
            validation.logError(
                `No valid MCAN TX pins found for peripheral ${inst.mcanPeripheral}`,
                inst, "mcanPeripheral"
            );
        }

        if (mcanRXOptions.length === 0) {
            validation.logError(
                `No valid MCAN RX pins found for peripheral ${inst.mcanPeripheral}`,
                inst, "mcanPeripheral"
            );
        }
    }
}

function validateSelectedPins(inst, validation) {
    // Validate UART pins
    if (!isValidPinForPeripheral(inst.uartTXPin, inst.uartPeripheral, "TX", uartTXPinData)) {
        validation.logError(
            `Selected pin ${inst.uartTXPin} is not valid for ${inst.uartPeripheral} TX`,
            inst, "uartTXPin"
        );
    }

    if (!isValidPinForPeripheral(inst.uartRXPin, inst.uartPeripheral, "RX", uartRXPinData)) {
        validation.logError(
            `Selected pin ${inst.uartRXPin} is not valid for ${inst.uartPeripheral} RX`,
            inst, "uartRXPin"
        );
    }

    // Validate I2C pins
    if (!isValidPinForPeripheral(inst.i2cSCLPin, inst.i2cPeripheral, "SCL", i2cSCLPinData)) {
        validation.logError(
            `Selected pin ${inst.i2cSCLPin} is not valid for ${inst.i2cPeripheral} SCL`,
            inst, "i2cSCLPin"
        );
    }

    if (!isValidPinForPeripheral(inst.i2cSDAPin, inst.i2cPeripheral, "SDA", i2cSDAPinData)) {
        validation.logError(
            `Selected pin ${inst.i2cSDAPin} is not valid for ${inst.i2cPeripheral} SDA`,
            inst, "i2cSDAPin"
        );
    }

    // Validate MCAN pins if supported
    if (deviceOptions.BSL_SUPPORT_MCAN) {
        if (!isValidPinForPeripheral(inst.mcanTXPin, inst.mcanPeripheral, "CANTX", mcanTXPinData)) {
            validation.logError(
                `Selected pin ${inst.mcanTXPin} is not valid for ${inst.mcanPeripheral} TX`,
                inst, "mcanTXPin"
            );
        }

        if (!isValidPinForPeripheral(inst.mcanRXPin, inst.mcanPeripheral, "CANRX", mcanRXPinData)) {
            validation.logError(
                `Selected pin ${inst.mcanRXPin} is not valid for ${inst.mcanPeripheral} RX`,
                inst, "mcanRXPin"
            );
        }
    }

    // Check for pin conflicts
    validatePinConflicts(inst, validation);
}

function isValidPinForPeripheral(pinName, peripheral, signalType, pinData) {
    // Handle empty or default pin cases
    if (pinName === "" || pinName === "0xFF") {
        return false;
    }

    // Find the pin in the pinData
    const pin = pinData.find(p => p.designSignalName === pinName);
    if (!pin) {
        return false;
    }

    // Check if the pin supports the required peripheral function
    const validMuxOption = pin.muxOptions.find(mux =>
        mux.peripheralPin.name === `${peripheral}.${signalType}`
    );

    return validMuxOption !== undefined;
}

function validatePinConflicts(inst, validation) {
    // Create array of all used pins and their purposes
    const usedPins = [
        { pin: inst.uartTXPin, purpose: "UART TX" },
        { pin: inst.uartRXPin, purpose: "UART RX" },
        { pin: inst.i2cSCLPin, purpose: "I2C SCL" },
        { pin: inst.i2cSDAPin, purpose: "I2C SDA" }
    ];

    if (deviceOptions.BSL_SUPPORT_MCAN) {
        usedPins.push(
            { pin: inst.mcanTXPin, purpose: "MCAN TX" },
            { pin: inst.mcanRXPin, purpose: "MCAN RX" }
        );
    }

    // Check for duplicates
    for (let i = 0; i < usedPins.length; i++) {
        for (let j = i + 1; j < usedPins.length; j++) {
            if (usedPins[i].pin !== "" &&
                usedPins[i].pin !== "0xFF" &&
                usedPins[i].pin === usedPins[j].pin) {
                validation.logError(
                    `Pin ${usedPins[i].pin} is used for both ${usedPins[i].purpose} and ${usedPins[j].purpose}`,
                    inst, [usedPins[i].pin, usedPins[j].pin]
                );
            }
        }
    }
}

/*  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  param inst    - a fully configured module instance
 *
 *  returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst) {
    return ([]);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with module
 *
 *  param component - hardware object describing signals and
 *                    resources they're attached to
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component) {
    return (true);
}

/*
 * Create password configs for SWD access, SWD commands (factory reset,
 * mass erase) - 128-bit passwords, and BSL access (256-bit).
 */
function createPWConfig(pwType, hiddenStatus, numPWRegs) {
    let pwConfig = [];

    // Map for password display names
    let pwMap = {
        "dbglockPW": "Debug Lock Password",
        "dbgnslockPW": "Non-Secure Debug Lock Password",
        "frPW": "Factory Reset",
        "mePW": "Mass Erase",
        "bslPW": "BSL Access",
        "badConvPW": "Bad Device Conversion Password"
    };

    // Map for password type-specific descriptions
    let pwDescriptionMap = {
        "dbglockPW": "Password for secure debug access control",
        "dbgnslockPW": "Password for non-secure debug access control",
        "frPW": "Password for factory reset operation",
        "mePW": "Password for mass erase operation",
        "bslPW": "Password for BSL access control",
        "badConvPW": "Password for device state conversion to BAD/DECOM"
    };

    // Map for password type-specific long descriptions
    let pwLongDescriptionMap = {
        "dbglockPW": `
Debug Lock Password Configuration:
- Used to protect debug access in secure mode
- Must be provided to enable debug access
- Cannot be recovered if lost
- Store securely for future debug access`,

        "dbgnslockPW": `
Non-Secure Debug Lock Password Configuration:
- Controls debug access in non-secure mode
- Required for non-secure debug operations
- Cannot be recovered if lost
- Store securely for future debug access`,

        "frPW": `
Factory Reset Password Configuration:
- Protects factory reset operation
- Enables controlled device reset
- Cannot be recovered if lost
- Store securely for maintenance purposes`,

        "mePW": `
Mass Erase Password Configuration:
- Protects mass erase operation
- Enables controlled flash erasure
- Cannot be recovered if lost
- Store securely for maintenance purposes`,

        "bslPW": `
BSL Access Password Configuration:
- Controls access to Bootstrap Loader
- Required for BSL operations
- Cannot be recovered if lost
- Store securely for future BSL access`,

        "badConvPW": `
WARNING: Bad Device Conversion Password Configuration

By enabling this option, you acknowledge:
1. This password will be used to protect device state conversion
2. The conversion process is IRREVERSIBLE
3. Once converted, the device cannot be restored to functional state
4. Password must be securely stored for future use
5. Incorrect password configuration may prevent intended conversion

This password is required for the DSSM_COMMAND_CONVERT_BAD_DEVICE command
which transitions the device from Functional State (FS) to DECOM state.`
    };

    // Default values based on password type and device
    let pwDefaults;
    if (pwType === "bslPW") {
        pwDefaults = [
            0x51409CE8, 0x88E11B97, 0x5CEEA650, 0xF930AF12,
            0x9FC3630A, 0x5A4AB4BD, 0x5F63720F, 0x761396AF
        ];
    } else {
        pwDefaults = [
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
        ];
    }

    for (let idx = 0; idx < numPWRegs; idx++) {
        pwConfig.push({
            name: pwType + idx,
            displayName: pwMap[pwType] + "[" + idx + "]",
            description: pwDescriptionMap[pwType] + ` (Word ${idx})`,
            longDescription: pwLongDescriptionMap[pwType] + `\n\nThis is word ${idx} of the ${numPWRegs}-word password.`,
            hidden: hiddenStatus,
            displayFormat: "hex",
            default: pwDefaults[idx],
            range: [0, 0xFFFFFFFF],
        });
    }
    return pwConfig;
}

function onChangeAPEnable(inst, ui) {
    onChangeSetCustomProfile(inst, ui);
    updateGUIAPEnable(inst, ui);
    updateGUIFactoryReset(inst, ui);
    updateGUIMassErase(inst, ui);
}

function updateGUIAPEnable(inst, ui) {
    switch (inst.apEnable) {
        case "level1":
            // Level 1: Show only secure debug password
            system.utils.showGroupConfig("GROUP_DEBUG_LOCK", inst, ui);
            system.utils.hideGroupConfig("GROUP_DEBUG_NS_LOCK", inst, ui);
            break;

        case "level2":
            // Level 2: Show both secure and non-secure debug passwords
            system.utils.showGroupConfig("GROUP_DEBUG_LOCK", inst, ui);
            system.utils.showGroupConfig("GROUP_DEBUG_NS_LOCK", inst, ui);
            break;

        case "enabled":
            // Level 0: Hide all password groups
            system.utils.hideGroupConfig("GROUP_DEBUG_LOCK", inst, ui);
            system.utils.hideGroupConfig("GROUP_DEBUG_NS_LOCK", inst, ui);
            break;

        case "disabled":
            // Disabled: Hide all password groups
            system.utils.hideGroupConfig("GROUP_DEBUG_LOCK", inst, ui);
            system.utils.hideGroupConfig("GROUP_DEBUG_NS_LOCK", inst, ui);
            break;
    }

    updateGUISWDPassword(inst, ui);
}

function onChangeSWDEnable(inst, ui) {
    onChangeSetCustomProfile(inst, ui);
}

function onChangeTIFAEnable(inst, ui) {
    onChangeSetCustomProfile(inst, ui);
}

function updateGUISWDPassword(inst, ui) {

    inst.dbglockPW0 = 0xFFFFFFFF;
    inst.dbglockPW1 = 0xFFFFFFFF;
    inst.dbglockPW2 = 0xFFFFFFFF;
    inst.dbglockPW3 = 0xFFFFFFFF;
    inst.dbglockPW4 = 0xFFFFFFFF;
    inst.dbglockPW5 = 0xFFFFFFFF;
    inst.dbglockPW6 = 0xFFFFFFFF;
    inst.dbglockPW7 = 0xFFFFFFFF;
    inst.dbgnslockPW0 = 0xFFFFFFFF;
    inst.dbgnslockPW1 = 0xFFFFFFFF;
    inst.dbgnslockPW2 = 0xFFFFFFFF;
    inst.dbgnslockPW3 = 0xFFFFFFFF;
    inst.dbgnslockPW4 = 0xFFFFFFFF;
    inst.dbgnslockPW5 = 0xFFFFFFFF;
    inst.dbgnslockPW6 = 0xFFFFFFFF;
    inst.dbgnslockPW7 = 0xFFFFFFFF;
}

function updateGUIFactoryReset(inst, ui) {
    if (inst.frMode == "enabledWithPW") {
        system.utils.showGroupConfig("GROUP_FACTORY_RESET_PW", inst, ui);
    }
    else {
        system.utils.hideGroupConfig("GROUP_FACTORY_RESET_PW", inst, ui);
    }

    updateGUIFRPassword(inst, ui);
}

function updateGUIFRPassword(inst, ui) {
    if (inst.frMode != "enabledWithPW") {
        /* Reset password field */
        inst.frPW0 = 0xFFFFFFFF;
        inst.frPW1 = 0xFFFFFFFF;
        inst.frPW2 = 0xFFFFFFFF;
        inst.frPW3 = 0xFFFFFFFF;
        inst.frPW4 = 0xFFFFFFFF;
        inst.frPW5 = 0xFFFFFFFF;
        inst.frPW6 = 0xFFFFFFFF;
        inst.frPW7 = 0xFFFFFFFF;
    }
}

function updateGUIMassErase(inst, ui) {
    if (inst.meMode == "enabledWithPW" && !(Common.isDeviceM0C())) {
        system.utils.showGroupConfig("GROUP_MASS_ERASE_PW", inst, ui);
    }
    else {
        system.utils.hideGroupConfig("GROUP_MASS_ERASE_PW", inst, ui);
    }

    updateGUIMEPassword(inst, ui);
}

function updateGUIMEPassword(inst, ui) {
    if (inst.meMode != "enabledWithPW") {
        /* Reset password field */
        inst.mePW0 = 0xFFFFFFFF;
        inst.mePW1 = 0xFFFFFFFF;
        inst.mePW2 = 0xFFFFFFFF;
        inst.mePW3 = 0xFFFFFFFF;
        inst.mePW4 = 0xFFFFFFFF;
        inst.mePW5 = 0xFFFFFFFF;
        inst.mePW6 = 0xFFFFFFFF;
        inst.mePW7 = 0xFFFFFFFF;
    }
}

function onChangeFactoryReset(inst, ui) {
    onChangeSetCustomProfile(inst, ui);
    updateGUIFactoryReset(inst, ui);
}

function onChangeMassErase(inst, ui) {
    onChangeSetCustomProfile(inst, ui);
    updateGUIMassErase(inst, ui);
}

function onChangePLL(inst, ui) {
    if (inst.syspllConfig === "0xFF") {
        // NOPLL mode - hide PLL-specific configurations
        system.utils.hideGroupConfig("GROUP_PLL_CUSTOM_CONFIG", inst, ui);
        // Still show common config as HFXT settings are needed even in NOPLL mode
        system.utils.showGroupConfig("GROUP_PLL_COMMON_CONFIG", inst, ui);
        // Hide PLL settling time when PLL is not used
        ui.syspllSettlingTime.hidden = true;
    } else {
        // PLL mode (either 80MHz or custom)
        system.utils.showGroupConfig("GROUP_PLL_COMMON_CONFIG", inst, ui);
        ui.syspllSettlingTime.hidden = false;

        if (inst.syspllConfig === "0xAA") { // PLL_80MHz
            // Hide custom configuration for fixed 80MHz mode
            system.utils.hideGroupConfig("GROUP_PLL_CUSTOM_CONFIG", inst, ui);

            // Set appropriate QDIV based on source
            if (inst.syspllSource === "0xAA") { // HFXT 20MHz
                inst.syspllConfigQDiv = 0x0F; // QDIV_HFCLK_IN_20MHZ_PLL_LOOP160
            } else if (inst.syspllSource === "0xBB") { // HFXT 40MHz
                inst.syspllConfigQDiv = 0x07; // QDIV_HFCLK_IN_40MHZ_PLL_LOOP160
            } else { // SYSOSC (default)
                inst.syspllConfigQDiv = 0x09; // QDIV_SYSOSC_PLL_LOOP160
            }

            // Fixed RDIV for 80MHz
            inst.syspllConfigRDiv = 0x00;

            // Make fields read-only in 80MHz mode
            ui.syspllConfigQDiv.readOnly = true;
            ui.syspllConfigRDiv.readOnly = true;

        } else { // PLL_CUSTOM
            // Show custom configuration fields
            system.utils.showGroupConfig("GROUP_PLL_CUSTOM_CONFIG", inst, ui);

            // Allow custom QDIV/RDIV values
            ui.syspllConfigQDiv.readOnly = false;
            ui.syspllConfigRDiv.readOnly = false;

            // Set default values if not already set
            if (!inst.syspllConfigQDiv) {
                inst.syspllConfigQDiv = 0x0F;
            }
            if (!inst.syspllConfigRDiv) {
                inst.syspllConfigRDiv = 0x00;
            }
        }

        // Always set default settling time if not already set
        if (!inst.syspllSettlingTime) {
            inst.syspllSettlingTime = 0xC8;
        }
    }

    // HFXT startup monitor time is needed whenever HFXT is selected
    if (inst.syspllSource === "0xAA" || inst.syspllSource === "0xBB") {
        ui.hfxtStartupMonitorTime.hidden = false;
        if (!inst.hfxtStartupMonitorTime) {
            inst.hfxtStartupMonitorTime = 0x08;
        }
    } else {
        ui.hfxtStartupMonitorTime.hidden = true;
    }

    // Validate configuration
    validatePLLConfig(inst, ui);
}

// Add validation function
function validatePLLConfig(inst, ui) {
    let inputFreq;
    if (inst.syspllSource === "0xAA") {
        inputFreq = 20; // 20MHz HFXT
    } else if (inst.syspllSource === "0xBB") {
        inputFreq = 40; // 40MHz HFXT
    } else {
        inputFreq = 16; // 16MHz SYSOSC
    }

    if (inst.syspllConfig !== "0xFF") {
        // Calculate output frequency
        const pDiv = 2; // Fixed divider
        const outputFreq = ((inputFreq / pDiv) * (inst.syspllConfigQDiv + 1)) /
            (inst.syspllConfigRDiv + 1);

        // Add warning if frequency exceeds device maximum
        if (outputFreq > 80) {
            ui.syspllConfigQDiv.warningText =
                `Configuration results in ${outputFreq}MHz which exceeds maximum of 80MHz`;
        } else {
            ui.syspllConfigQDiv.warningText = undefined;
        }
    }
}

function onChangeBSLEnable(inst, ui) {
    updateGUIBSLEnable(inst, ui);
    updateGUIGPIOInvoke(inst, ui);
    // updateGUIPluginConfig(inst, ui);
}

function updateGUIBSLEnable(inst, ui) {
    if (inst.bslMode) {
        system.utils.showGroupConfig("GROUP_BSL", inst, ui);

        inst.bslInvokePinCheck = true;
        inst.bslDefaultInvokePin = true;
    }
    else {
        system.utils.hideGroupConfig("GROUP_BSL", inst, ui);

        inst.bslInvokePinCheck = false;
        inst.bslDefaultInvokePin = false;
    }

    /* Reset fields to default */
    ui.bslInvokePinData0.hidden = true;
    ui.bslInvokePinData1.hidden = true;

    inst.uartTXPin = defaultBSLUARTTX;
    inst.uartRXPin = defaultBSLUARTRX;
    inst.i2cSCLPin = defaultBSLI2CSCL;
    inst.i2cSDAPin = defaultBSLI2CSDA;
    inst.i2cSlaveAddress = 0x0048


    inst.bslConfigID = 0x05000000;
    inst.bslAppVersion = 0xFFFFFFFF;
    inst.bslEnableReadOut = false;
    inst.bslSecurityConfig = "triggerFactoryReset";

    updateGUIBSLPassword(inst, ui);
}

function updateGUIBSLPassword(inst, ui) {
    if (!inst.bslMode) {
        /* Reset password */
        if (Common.isDeviceFamily_PARENT_MSPM33C321X()) {
            inst.bslPW0 = 0x51409CE8;
            inst.bslPW1 = 0x88E11B97;
            inst.bslPW2 = 0x5CEEA650;
            inst.bslPW3 = 0xF930AF12;
            inst.bslPW4 = 0x9FC3630A;
            inst.bslPW5 = 0x5A4AB4BD;
            inst.bslPW6 = 0x5F63720F;
            inst.bslPW7 = 0x761396AF;
        }
        else if (Common.isDeviceFamily_PARENT_MSPM0L122X_L222X()) {
            inst.bslPW0 = 0x761396AF;
            inst.bslPW1 = 0x5F63720F;
            inst.bslPW2 = 0x5A4AB4BD;
            inst.bslPW3 = 0x9FC3630A;
            inst.bslPW4 = 0xF930AF12;
            inst.bslPW5 = 0x5CEEA650;
            inst.bslPW6 = 0x88E11B97;
            inst.bslPW7 = 0x51409CE8;
        }
        else {
            inst.bslPW0 = 0xFFFFFFFF;
            inst.bslPW1 = 0xFFFFFFFF;
            inst.bslPW2 = 0xFFFFFFFF;
            inst.bslPW3 = 0xFFFFFFFF;
            inst.bslPW4 = 0xFFFFFFFF;
            inst.bslPW5 = 0xFFFFFFFF;
            inst.bslPW6 = 0xFFFFFFFF;
            inst.bslPW7 = 0xFFFFFFFF;
        }
    }
}

function updateGUIGPIOInvoke(inst, ui) {
    updateGUIGPIOInvokePinCheck(inst, ui);
    updateGUIGPIODefaultInvokePin(inst, ui);
}

function updateGUIGPIOInvokePinCheck(inst, ui) {
    ui.bslDefaultInvokePin.hidden = !(inst.bslInvokePinCheck);

    if (!inst.bslInvokePinCheck) {
        /* If the invoke pin check is disabled, reset values to default */
        inst.bslDefaultInvokePin = true;
        inst.bslInvokePin = filteredPinOptions[0].name;
        inst.bslInvokePinLevel = 0;
    }
}

function updateGUIGPIODefaultInvokePin(inst, ui) {
    if (inst.bslInvokePinCheck && !inst.bslDefaultInvokePin) {
        ui.bslInvokePin.hidden = false;
        ui.bslInvokePINCM.hidden = false;
        ui.bslInvokePinLevel.hidden = false;
    }
    else {
        ui.bslInvokePin.hidden = true;
        ui.bslInvokePINCM.hidden = true;
        ui.bslInvokePinLevel.hidden = true;

        /* Reset GPIO pin details to default */
        inst.bslInvokePin = filteredPinOptions[0].name;
        inst.bslInvokePinLevel = 0;
    }
}

function updateGUIPluginConfig(inst, ui) {
    ui.bslPluginType.hidden = !(inst.bslFlashPluginEnable);
    ui.bslPluginSRAMSize.hidden = !(inst.bslFlashPluginEnable);
    ui.bslPluginHookInit.hidden = !(inst.bslFlashPluginEnable);
    ui.bslPluginHookReceive.hidden = !(inst.bslFlashPluginEnable);
    ui.bslPluginHookTransmit.hidden = !(inst.bslFlashPluginEnable);
    ui.bslPluginHookDeInit.hidden = !(inst.bslFlashPluginEnable);

    updateGUIPluginHooks(inst, ui);
}

function updateGUIPluginHooks(inst, ui) {
    if (!inst.bslFlashPluginEnable) {
        inst.bslPluginType = "bslPluginAny";
        inst.bslPluginSRAMSize = 0xFF;
        inst.bslPluginHookInit = "";
        inst.bslPluginHookInitAddress = 0xFFFFFFFF;
        inst.bslPluginHookReceive = "";
        inst.bslPluginHookReceiveAddress = 0xFFFFFFFF;
        inst.bslPluginHookTransmit = "";
        inst.bslPluginHookTransmitAddress = 0xFFFFFFFF;
        inst.bslPluginHookDeInit = "";
        inst.bslPluginHookDeInitAddress = 0xFFFFFFFF;
        ui.bslPluginHookInitAddress.hidden = true;
        ui.bslPluginHookReceiveAddress.hidden = true;
        ui.bslPluginHookTransmitAddress.hidden = true;
        ui.bslPluginHookDeInitAddress.hidden = true;
    }
}

function onChangePluginConfig(inst, ui) {
    updateGUIPluginConfig(inst, ui);
}

/************************* Profiles functions *******************************/
let securityLevel0Profile = {
    name: "LEVEL_0",
    apEnable: "enabled",
    swdEnable: true,
    frMode: "enabled",
};

if (!Common.isDeviceM0C()) {
    securityLevel0Profile = { ...securityLevel0Profile, ...{ tifaEnable: true }, ...{ meMode: "enabled" } }
}

let securityLevel1Profile = {
    name: "LEVEL_1",
    apEnable: "level1",
    swdEnable: true,
    frMode: "enabled",
};

let securityLevel2Profile = {
    name: "LEVEL_2",
    apEnable: "level2",
    swdEnable: false,
};

const profilesDebugSecurity = [
    securityLevel0Profile,
    securityLevel1Profile,
    securityLevel2Profile,
];

function onChangeNONMAINProfile(inst, ui) {
    if (inst.profile != "CUSTOM") {
        let selectedProfileArray = profilesDebugSecurity.filter(obj => { return obj.name === inst.profile });
        if (selectedProfileArray.length != 0) {
            const selectedProfile = Object.assign({}, selectedProfileArray[0]);
            delete selectedProfile.name;
            Object.assign(inst, selectedProfile)
        }
    }
    else {
        inst.swdEnable = true;
    }
}

function onChangeSetCustomProfile(inst, ui) {
    if (inst.swdEnable) {
        inst.profile = "CUSTOM";
    }
    else {
        /* If swdEnable is set to false, that is security level 2 */
        inst.profile = "LEVEL_2";
    }
}

function updateBank0SecurityProtection(inst, ui) {
    // Start with all bits clear (secure/protected)
    let protectionValueA = Number(0x00000000);
    let protectionValueB = Number(0x00000000);

    if (inst.bank0SecurityProtectSectors && inst.bank0SecurityProtectSectors.length > 0) {
        inst.bank0SecurityProtectSectors.forEach(sector => {
            if (sector.startsWith("sector")) {
                const sectorNum = parseInt(sector.replace("sector", ""));
                if (sectorNum >= 0 && sectorNum < 32) {
                    // Set bit to 1 for non-secure access
                    protectionValueA = (protectionValueA | (1 << sectorNum)) >>> 0;
                }
            } else if (sector.startsWith("block")) {
                const blockNum = parseInt(sector.replace("block", "")) - 32;
                if (blockNum >= 0 && blockNum < 32) {
                    // Set bit to 1 for non-secure access
                    protectionValueB = (protectionValueB | (1 << blockNum)) >>> 0;
                }
            }
        });
    }

    inst.bank0SecurityProtectionA = protectionValueA >>> 0;
    inst.bank0SecurityProtectionB = protectionValueB >>> 0;
}

function updateBank1SecurityProtection(inst, ui) {
    // Start with all bits clear (secure/protected)
    let protectionValueA = Number(0x00000000);
    let protectionValueB = Number(0x00000000);

    if (inst.bank1SecurityProtectSectors && inst.bank1SecurityProtectSectors.length > 0) {
        inst.bank1SecurityProtectSectors.forEach(sector => {
            if (sector.startsWith("sector")) {
                const sectorNum = parseInt(sector.replace("sector", ""));
                if (sectorNum >= 0 && sectorNum < 32) {
                    // Set bit to 1 for non-secure access
                    protectionValueA = (protectionValueA | (1 << sectorNum)) >>> 0;
                }
            } else if (sector.startsWith("block")) {
                const blockNum = parseInt(sector.replace("block", "")) - 32;
                if (blockNum >= 0 && blockNum < 32) {
                    // Set bit to 1 for non-secure access
                    protectionValueB = (protectionValueB | (1 << blockNum)) >>> 0;
                }
            }
        });
    }

    inst.bank1SecurityProtectionA = protectionValueA >>> 0;
    inst.bank1SecurityProtectionB = protectionValueB >>> 0;
}

function updateBank0PrivilegeProtection(inst, ui) {
    // Start with all bits clear (privileged/protected)
    let protectionValueA = Number(0x00000000);
    let protectionValueB = Number(0x00000000);

    if (inst.bank0PrivilegeProtectSectors && inst.bank0PrivilegeProtectSectors.length > 0) {
        inst.bank0PrivilegeProtectSectors.forEach(sector => {
            if (sector.startsWith("sector")) {
                const sectorNum = parseInt(sector.replace("sector", ""));
                if (sectorNum >= 0 && sectorNum < 32) {
                    // Set bit to 1 for non-privileged access
                    protectionValueA = (protectionValueA | (1 << sectorNum)) >>> 0;
                }
            } else if (sector.startsWith("block")) {
                const blockNum = parseInt(sector.replace("block", "")) - 32;
                if (blockNum >= 0 && blockNum < 32) {
                    // Set bit to 1 for non-privileged access
                    protectionValueB = (protectionValueB | (1 << blockNum)) >>> 0;
                }
            }
        });
    }

    inst.bank0PrivilegeProtectionA = protectionValueA >>> 0;
    inst.bank0PrivilegeProtectionB = protectionValueB >>> 0;
}

function updateBank1PrivilegeProtection(inst, ui) {
    // Start with all bits clear (privileged/protected)
    let protectionValueA = Number(0x00000000);
    let protectionValueB = Number(0x00000000);

    if (inst.bank1PrivilegeProtectSectors && inst.bank1PrivilegeProtectSectors.length > 0) {
        inst.bank1PrivilegeProtectSectors.forEach(sector => {
            if (sector.startsWith("sector")) {
                const sectorNum = parseInt(sector.replace("sector", ""));
                if (sectorNum >= 0 && sectorNum < 32) {
                    // Set bit to 1 for non-privileged access
                    protectionValueA = (protectionValueA | (1 << sectorNum)) >>> 0;
                }
            } else if (sector.startsWith("block")) {
                const blockNum = parseInt(sector.replace("block", "")) - 32;
                if (blockNum >= 0 && blockNum < 32) {
                    // Set bit to 1 for non-privileged access
                    protectionValueB = (protectionValueB | (1 << blockNum)) >>> 0;
                }
            }
        });
    }

    inst.bank1PrivilegeProtectionA = protectionValueA >>> 0;
    inst.bank1PrivilegeProtectionB = protectionValueB >>> 0;
}

function updateBank0WriteEraseProtection(inst, ui) {
    // Start with all bits set (no protection)
    let protectionValueA = Number(0xFFFFFFFF); // Initialize as Number
    let protectionValueB = Number(0xFFFFFFFF);

    // Process selected sectors
    if (inst.bank0WriteEraseProtectSectors && inst.bank0WriteEraseProtectSectors.length > 0) {
        inst.bank0WriteEraseProtectSectors.forEach(sector => {
            if (sector.startsWith("sector")) {
                const sectorNum = parseInt(sector.replace("sector", ""));
                if (sectorNum >= 0 && sectorNum < 32) {
                    // Ensure unsigned operations
                    const mask = (~(1 << sectorNum)) >>> 0;
                    protectionValueA = (protectionValueA & mask) >>> 0;
                }
            } else if (sector.startsWith("block")) {
                const blockNum = parseInt(sector.replace("block", "")) - 32;
                if (blockNum >= 0 && blockNum < 32) {
                    const mask = (~(1 << blockNum)) >>> 0;
                    protectionValueB = (protectionValueB & mask) >>> 0;
                }
            }
        });
    }

    // Ensure final values are unsigned
    inst.bank0WriteEraseProtectionA = protectionValueA >>> 0;
    inst.bank0WriteEraseProtectionB = protectionValueB >>> 0;
}

function updateBank1WriteEraseProtection(inst, ui) {
    // Start with all bits set (no protection)
    let protectionValueA = Number(0xFFFFFFFF); // Initialize as Number
    let protectionValueB = Number(0xFFFFFFFF);

    // Process selected sectors
    if (inst.bank1WriteEraseProtectSectors && inst.bank1WriteEraseProtectSectors.length > 0) {
        inst.bank1WriteEraseProtectSectors.forEach(sector => {
            if (sector.startsWith("sector")) {
                const sectorNum = parseInt(sector.replace("sector", ""));
                if (sectorNum >= 0 && sectorNum < 32) {
                    // Ensure unsigned operations
                    const mask = (~(1 << sectorNum)) >>> 0;
                    protectionValueA = (protectionValueA & mask) >>> 0;
                }
            } else if (sector.startsWith("block")) {
                const blockNum = parseInt(sector.replace("block", "")) - 32;
                if (blockNum >= 0 && blockNum < 32) {
                    const mask = (~(1 << blockNum)) >>> 0;
                    protectionValueB = (protectionValueB & mask) >>> 0;
                }
            }
        });
    }

    // Ensure final values are unsigned
    inst.bank1WriteEraseProtectionA = protectionValueA >>> 0;
    inst.bank1WriteEraseProtectionB = protectionValueB >>> 0;

}


function updateDBANKWriteEraseProtection(inst, ui) {
    // Start with all bits set (no protection)
    let protectionValue = 0xFFFF;  // Only 16 bits used

    if (inst.dbankWriteEraseProtectSectors && inst.dbankWriteEraseProtectSectors.length > 0) {
        inst.dbankWriteEraseProtectSectors.forEach(sector => {
            if (sector.startsWith("sector")) {
                const sectorNum = parseInt(sector.replace("sector", ""));
                if (sectorNum >= 0 && sectorNum < 16) {
                    const mask = (~(1 << sectorNum)) & 0xFFFF;
                    protectionValue = (protectionValue & mask) & 0xFFFF;
                }
            }
        });
    }

    inst.dbankWriteEraseProtection = protectionValue;
}

function updateDBANKSecurityProtection(inst, ui) {
    // Start with all bits clear (secure)
    let protectionValue = 0x0000;  // Only 16 bits used

    if (inst.dbankSecurityProtectSectors && inst.dbankSecurityProtectSectors.length > 0) {
        inst.dbankSecurityProtectSectors.forEach(sector => {
            if (sector.startsWith("sector")) {
                const sectorNum = parseInt(sector.replace("sector", ""));
                if (sectorNum >= 0 && sectorNum < 16) {
                    // Set bit to 1 for non-secure access
                    protectionValue = (protectionValue | (1 << sectorNum)) & 0xFFFF;
                }
            }
        });
    }

    inst.dbankSecurityProtection = protectionValue;
}

function updateDBANKPrivilegeProtection(inst, ui) {
    // Start with all bits clear (privileged)
    let protectionValue = 0x0000;  // Only 16 bits used

    if (inst.dbankPrivilegeProtectSectors && inst.dbankPrivilegeProtectSectors.length > 0) {
        inst.dbankPrivilegeProtectSectors.forEach(sector => {
            if (sector.startsWith("sector")) {
                const sectorNum = parseInt(sector.replace("sector", ""));
                if (sectorNum >= 0 && sectorNum < 16) {
                    // Set bit to 1 for non-privileged access
                    protectionValue = (protectionValue | (1 << sectorNum)) & 0xFFFF;
                }
            }
        });
    }

    inst.dbankPrivilegeProtection = protectionValue;
}


/* Password Configurables: Device Specific */
const passwordWordLen = 8;
let mePWConfig = createPWConfig("mePW", true, passwordWordLen);
let frPWConfig = createPWConfig("frPW", true, passwordWordLen);
let debuglockPWConfig = createPWConfig("dbglockPW", true, passwordWordLen);
let debugNSlockPWConfig = createPWConfig("dbgnslockPW", true, passwordWordLen);
let badConvPWConfig = createPWConfig("badConvPW", false, passwordWordLen);
let nonMainConfig = [
    /****** PROFILES CONFIGURATION *******/
    {
        name: "GROUP_PROFILE",
        displayName: "Quick Profiles",
        description: "",
        collapsed: false,
        config: [
            {
                name: "nonmainWarning",
                displayName: "Accept configuration risks",
                description: "Acknowledge Configuration NVM (NONMAIN) configuration risks",
                longDescription: `
For security purposes, MSPM0 devices require a valid Configuration NVM(NONMAIN)
at all times.When updating Configuration NVM, the old Configuration NVM
configuration is erased, and the new configuration is programmed.Any
interruption during the erase or re - program operations i.e.unplugging device,
                    removing SWD jumpers, hitting reset by accident, cancelling the code download,
                        an IDE crash, etc.can brick the device permanently.Improper configuration of
Configuration NVM can also lead to permanent locking of the device.

To minimize these risks, TI ** highly recommends ** adding and configuring
                this module only when one is ready to go to production.Click the checkbox to
acknowledge the risks when using Configuration NVM, and refer to the relevant
IDE guides in the Tools documentation directory in the SDK on how to properly
program Configuration NVM.
`,
                default: false,
            },
            {
                name: "profile",
                displayName: "Debug Security Profiles",
                description: 'Pre-defined profiles for debug security configurations',
                longDescription: `
To accommodate a variety of needs while keeping the configuration process
                simple, MSPM0 devices support three generic security levels: no restrictions(level 0),
                    custom restrictions(level 1), and fully restricted(level 2).

Refer to the [MSPM33 Security User Guide](https://www.ti.com/lit/ug/sluudg7/sluudg7.pdf)
for more details on the security levels.

                    The Quick Profile Options are:
                        * ** Security Level 0 **:
    * Physical debug port / SW - DP: enabled
                    * Application debug: enabled
                        * Mass erase policy: enabled
                            * Factory reset policy: enabled
                                * TI failure analysis policy: enabled
                                    * ** Security Level 2 **:
    * Physical debug port / SW - DP: disabled
                    * Application debug: don't care, access not possible through SWD
                        * Mass erase policy: don't care from SWD, but can be done through BSL if
      BSL is enabled
                    * Factory reset policy: don't care from SWD, but can be done through BSL if
      BSL is enabled
                    * TI failure analysis policy: don't care, access not possible through SWD
                        * ** Security Level 1(Custom) **:
    * Allows custom configuration.`,
                hidden: true,
                default: "LEVEL_0",
                options: [
                    { name: "LEVEL_0", displayName: "Security Level 0 - No restrictions" },
                    { name: "LEVEL_2", displayName: "Security Level 2 - Fully restrictive" },
                    { name: "CUSTOM", displayName: "Security Level 1 - Custom restrictions" },
                ],
                onChange: onChangeNONMAINProfile,
            },
        ],
    },
    /****** BCR CONFIGURATION *******/
    {
        name: "GROUP_BCR",
        displayName: "Boot Configuration Routine (BCR) Configuration",
        description: "",
        longDescription: `
The BCR is the first firmware to run on the device after a BOOTRST.The BCR manages the
following at boot time:

* Configuring the debug interface security policy
                    * Configuring the flash memory static write protection policy
                        * Optionally:
    * Executing a mass erase
                    * Executing a factory reset
                        * Verifying the integrity of some / all of the application firmware with
      32 - bit CRC
                    * Enabling fast boot mode
                        * Starting the BSL
                            `,
        collapsed: true,
        config: [
            {
                name: "GROUP_DEBUG_SECURITY_POLICY_CONFIG",
                displayName: "Debug Security Policy Configuration",
                description: "",
                collapsed: true,
                config: [
                    // .swdpMode
                    {
                        name: "swdEnable",
                        displayName: "Enable Physical Debug Port (SW-DP)",
                        description: "Selects if the physical debug port (SW-DP) is enabled or completely disabled.",
                        longDescription: `
When this field is disabled, the physical debug port(SW - DP) is completely
                disabled, and ** all ** of the SWD - accessible functions(application debug, mass
erase, factory reset, and TI failure analysis) are not accessible through SWD,
                    regardless of their individual configuration.

** Note **: If the BSL is enabled, the mass erase and factory reset configuration
fields are still used by the BSL to authorize mass erase or factory reset
commands originating from the BSL interface.

This is the ** fully restrictive ** state.
This should be used for mass production when no further access to any SWD
functions is required and a fully secure state is desired for the device.

Refer to the [MSPM33 Security User Guide](https://www.ti.com/lit/ug/sluudg7/sluudg7.pdf)
for more details on the security levels.
`,
                        hidden: false,
                        default: true,
                        onChange: onChangeSWDEnable,
                    },
                    // .debugAccess
                    {
                        name: "apEnable",
                        displayName: "Enable Application Debug Access",
                        description: "Selects the application debug access level: Level 0 (Full Access), Level 1, Level 2, or Disabled.",
                        longDescription: `
                    Application debug access includes:

                        * Full access to the processor, memory map, and peripherals through the AHB-AP
                            * Access to the device EnergyTrace + state information through the ET - AP
                                * Access to the device power state controls for debug through the PWR - AP

The available options are:
* **Level 0 - Full Access**: No restrictions on application debug access.
* **Level 1 - Secure Protected, Non-Secure Enabled**: Secure debug is protected, non-secure debug is enabled. Well suited for restricted prototyping / development scenarios and for mass production scenarios where the desire is to retain certain SWD functions such as factory reset and TI failure analysis while disabling other functions (such as application debug).
* **Level 2 - Both Secure and Non-Secure Protected**: Both secure and non-secure debug access are protected.
* **Disabled**: Application debug access is completely disabled.

Refer to the [MSPM33 Security User Guide](https://www.ti.com/lit/ug/sluudg7/sluudg7.pdf)
for more details on the security levels.
`,
                        hidden: false,
                        default: "enabled",
                        options: enableOptions,
                        getDisabledOptions: getDisabledEnableOptions,
                        onChange: onChangeAPEnable,
                    },
                    {
                        // .passwordDebugLock
                        /* SWDPW[0-3] */
                        name: "GROUP_DEBUG_LOCK",
                        displayName: "Secure Debug Password",
                        collapsed: false,
                        longDescription: sha256Note,
                        config: debuglockPWConfig,
                    },
                    {
                        // .passwordDebugLock
                        /* SWDPW[0-3] */
                        name: "GROUP_DEBUG_NS_LOCK",
                        displayName: "Non-secure Debug Password",
                        collapsed: false,
                        longDescription: sha256Note,
                        config: debugNSlockPWConfig,
                    },
                ],
            },
            {
                name: "GROUP_SWD_MASS_ERASE_FACTORY_RESET_CONFIG",
                displayName: "Mass Erase and Factory Reset Configuration",
                description: "",
                collapsed: true,
                config: [
                    {
                        // .factoryResetMode
                        /* BOOTCFG2.FRMODE */
                        name: "frMode",
                        displayName: "Factory Reset Mode Policy",
                        description: "Sets the factory reset mode policy",
                        longDescription: `
The BCR provides factory reset functionality through commands sent to the
device over SWD from a debug probe using the debug subsystem mailbox (DSSM).
This command is not available in SWD security level 2 but is optionally
                available in security levels 0 and 1.

The factory reset command can be configured to be enabled, enabled with an
unique 128 - bit password, or disabled.

A SWD factory reset is an erase of the MAIN flash regions followed by a reset
of the NONMAIN flash region to default values.This erase is useful for
completely resetting the BCR and BSL device boot policies while also erasing
application code and data.
`,
                        hidden: false,
                        default: "enabled",
                        options: factoryResetOptions,
                        getDisabledOptions: getDisabledEnableOptions,
                        onChange: onChangeFactoryReset,
                    },
                    {
                        // .passwordFactoryReset
                        /* FRPW[0-3] */
                        name: "GROUP_FACTORY_RESET_PW",
                        displayName: "Factory Reset Password",
                        collapsed: false,
                        longDescription: sha256Note,
                        config: frPWConfig,
                    },
                    {
                        // .massEraseMode
                        /* BOOTCFG2.MEMODE */
                        name: "meMode",
                        displayName: "Mass Erase Mode Policy",
                        description: "Sets the mass erase mode policy",
                        longDescription: warningNoteM0C + `
The BCR provides mass erase functionality through commands sent to the
device over SWD from a debug probe using the debug subsystem mailbox(DSSM).
This command is not available in SWD security level 2 but is optionally
available in security levels 0 and 1.

The mass erase command can be configured to be enabled, enabled with an
unique 128 - bit password, or disabled.

A SWD mass erase is an erase of the MAIN flash regions only, which typically
includes the user application.The BCR and BSL policies stored in NONMAIN
flash are not affected.This erase is useful for erasing all application code
and data while leaving the device configuration intact.
`,
                        hidden: false,
                        default: "enabled",
                        readOnly: Common.isDeviceM0C(),
                        options: massEraseOptions,
                        getDisabledOptions: getDisabledEnableOptions,
                        onChange: onChangeMassErase,
                    },
                    {
                        // .passwordMassErase
                        /* MEPW[0-3] */
                        name: "GROUP_MASS_ERASE_PW",
                        displayName: "Mass Erase Password",
                        collapsed: false,
                        longDescription: sha256Note,
                        config: mePWConfig,
                    },
                ],
            },
            {
                name: "GROUP_FLASH_SECURITY",
                displayName: "Flash Security Configuration",
                description: "Configure security settings for different flash memory regions",
                longDescription: `
Flash memory in MSPM0 devices can be protected at multiple levels:

1. Write/Erase Protection:
   - Prevents modification of flash contents
   - Can be configured per sector/block
   - Critical for protecting bootloader and application code
   - Cannot be bypassed by application code

2. Security Protection (Secure/Non-Secure):
   - Controls access based on security state
   - Secure code can access both secure and non-secure regions
   - Non-secure code can only access non-secure regions

3. Privilege Protection:
   - Controls access based on privilege level
   - Privileged code can access all regions
   - Unprivileged code can only access non-privileged regions
   - Used for separation of OS and application code

Flash Organization:
- Bank 0 and Bank 1 are main application flash banks
- Each bank has:
  * 32 x 2KB sectors (0-31)
  * 32 x 16KB blocks (32-63)
- Protection can be set independently for each sector/block
`,
                collapsed: true,
                config: [
                    {
                        name: "nonMainWriteProtection",
                        displayName: "NonMain Flash Write Protection",
                        description: "Controls write protection for NonMain flash sectors",
                        default: "disabled",
                        options: [
                            { name: "disabled", displayName: "Disabled" },
                            { name: "enabled", displayName: "Enabled - Protect BCR/BSL Config)" }
                        ],
                    },
                    {
                        name: "GROUP_BANK0_PROTECTION",
                        displayName: "Bank 0 Flash Protection",
                        description: "Configure protection settings for Bank 0 flash memory",
                        collapsed: true,
                        config: [
                            {
                                name: "bank0WriteEraseProtectSectors",
                                displayName: "Write/Erase Protected Sectors",
                                description: "Select sectors to be write/erase protected",
                                default: [],
                                minSelections: 0,
                                longDescription: `
Select sectors to enable write/erase protection:
- Protected sectors cannot be written or erased
- Each sector can be individually protected
- First 32 sectors are 2KB each
- Remaining sectors are in 16KB blocks

Memory Map:
Sector  Address Range          Size
0       0x00000000-0x000007FF  2KB
1       0x00000800-0x00000FFF  2KB
...
31      0x00007800-0x00007FFF  2KB
32      0x00008000-0x0000BFFF  16KB
...
`,
                                options: BANK0_SECTORS,
                                onChange: (inst, ui) => {
                                    updateBank0WriteEraseProtection(inst, ui);
                                }
                            },
                            {
                                name: "bank0WriteEraseProtectionA",
                                displayName: "Write/Erase Protection Value A",
                                description: "Generated protection value for first 32 sectors",
                                displayFormat: "hex",
                                default: 0xFFFFFFFF,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "bank0WriteEraseProtectionB",
                                displayName: "Write/Erase Protection Value B",
                                description: "Generated protection value for remaining sectors",
                                displayFormat: "hex",
                                default: 0xFFFFFFFF,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "bank0SecurityProtectSectors",
                                displayName: "Security Protected Sectors",
                                description: "Select sectors to be security protected (secure)",
                                longDescription: `
                    Security Protection Configuration:
                    - Each bit represents a sector
                    - First 32 sectors (A): Each bit represents one 2KB sector
                    - Remaining sectors (B): Each bit represents eight 2KB sectors

                    Bit Values:
                    1: Sector is NON-SECURE
                    0: Sector is SECURE`,
                                options: BANK0_SECTORS,
                                default: [],
                                minSelections: 0,
                                onChange: (inst, ui) => {
                                    updateBank0SecurityProtection(inst, ui);
                                }
                            },
                            {
                                name: "bank0SecurityProtectionA",
                                displayFormat: "hex",
                                default: 0x00000000,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "bank0SecurityProtectionB",
                                displayFormat: "hex",
                                default: 0x00000000,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "bank0PrivilegeProtectSectors",
                                displayName: "Privilege Protected Sectors",
                                description: "Select sectors to be privilege protected",
                                longDescription: `
                    Privilege Protection Configuration:
                    - Each bit represents a sector
                    - First 32 sectors (A): Each bit represents one 2KB sector
                    - Remaining sectors (B): Each bit represents eight 2KB sectors

                    Bit Values:
                    1: Sector is NON-PRIVILEGED
                    0: Sector is PRIVILEGED`,
                                default: [],
                                options: BANK0_SECTORS,
                                minSelections: 0,
                                onChange: (inst, ui) => {
                                    updateBank0PrivilegeProtection(inst, ui);
                                }
                            },
                            {
                                name: "bank0PrivilegeProtectionA",
                                displayFormat: "hex",
                                default: 0x00000000,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "bank0PrivilegeProtectionB",
                                displayFormat: "hex",
                                default: 0x00000000,
                                readOnly: true,
                                hidden: true
                            }
                        ]
                    },
                    {
                        name: "GROUP_BANK1_PROTECTION",
                        displayName: "Bank 1 Flash Protection",
                        description: "Configure protection settings for Bank 1 flash memory",
                        longDescription: `
                    Bank 1 Flash Memory Protection Configuration:
                    - Each bit represents a sector or group of sectors
                    - First 32 sectors (A): Each bit represents one 2KB sector
                    - Remaining sectors (B): Each bit represents eight 2KB sectors

                    Protection Types:
                    1. Write/Erase Protection:
                       - 1: Sector is NOT write/erase protected
                       - 0: Sector is write/erase protected
                    `,
                        collapsed: true,
                        config: [
                            {
                                name: "bank1WriteEraseProtectSectors",
                                displayName: "Write/Erase Protected Sectors",
                                description: "Select sectors to be write/erase protected",
                                default: [],
                                minSelections: 0,
                                longDescription: `
Select sectors to enable write/erase protection:
- Protected sectors cannot be written or erased
- Each sector can be individually protected
- First 32 sectors are 2KB each
- Remaining sectors are in 16KB blocks

Memory Map:
Sector  Address Range          Size
0       0x00000000-0x000007FF  2KB
1       0x00000800-0x00000FFF  2KB
...
31      0x00007800-0x00007FFF  2KB
32      0x00008000-0x0000BFFF  16KB
...
`,
                                options: BANK1_SECTORS,
                                onChange: (inst, ui) => {
                                    updateBank1WriteEraseProtection(inst, ui);
                                }
                            },
                            {
                                name: "bank1WriteEraseProtectionA",
                                displayName: "Write/Erase Protection Value A",
                                description: "Generated protection value for first 32 sectors",
                                displayFormat: "hex",
                                default: 0xFFFFFFFF,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "bank1WriteEraseProtectionB",
                                displayName: "Write/Erase Protection Value B",
                                description: "Generated protection value for remaining sectors",
                                displayFormat: "hex",
                                default: 0xFFFFFFFF,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "bank1SecurityProtectSectors",
                                displayName: "Security Protected Sectors",
                                description: "Select sectors to be security protected (secure)",
                                longDescription: `
            Security Protection Configuration:
            - Each bit represents a sector
            - First 32 sectors (A): Each bit represents one 2KB sector
            - Remaining sectors (B): Each bit represents eight 2KB sectors

            Bit Values:
            1: Sector is NON-SECURE
            0: Sector is SECURE`,
                                default: [],
                                options: BANK1_SECTORS,
                                default: [],
                                minSelections: 0,
                                onChange: (inst, ui) => {
                                    updateBank1SecurityProtection(inst, ui);
                                }
                            },
                            {
                                name: "bank1SecurityProtectionA",
                                displayFormat: "hex",
                                default: 0x00000000,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "bank1SecurityProtectionB",
                                displayFormat: "hex",
                                default: 0x00000000,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "bank1PrivilegeProtectSectors",
                                displayName: "Privilege Protected Sectors",
                                description: "Select sectors to be privilege protected",
                                longDescription: `
            Privilege Protection Configuration:
            - Each bit represents a sector
            - First 32 sectors (A): Each bit represents one 2KB sector
            - Remaining sectors (B): Each bit represents eight 2KB sectors

            Bit Values:
            1: Sector is NON-PRIVILEGED
            0: Sector is PRIVILEGED`,
                                default: [],
                                options: BANK1_SECTORS,
                                default: [],
                                minSelections: 0,
                                onChange: (inst, ui) => {
                                    updateBank1PrivilegeProtection(inst, ui);
                                }
                            },
                            {
                                name: "bank1PrivilegeProtectionA",
                                displayFormat: "hex",
                                default: 0x00000000,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "bank1PrivilegeProtectionB",
                                displayFormat: "hex",
                                default: 0x00000000,
                                readOnly: true,
                                hidden: true
                            }
                        ]
                    },
                    {
                        name: "GROUP_DBANK_PROTECTION",
                        displayName: "DBANK Flash Protection",
                        description: "Configure protection settings for DBANK flash memory",
                        longDescription: `
                    DBANK Flash Memory Protection Configuration:

                    Memory Map:
                    - Total Size: 32KB
                    - 16 sectors of 2KB each
                    - Address Range: 0x80000000 - 0x80007FFF

                    Protection Types:
                    1. Write/Erase Protection:
                       - Prevents flash modification
                       - Per sector granularity
                       - Cannot be bypassed by software

                    2. Security Protection:
                       - Controls Secure/Non-secure access
                       - Used with TrustZone-M
                       - Separates secure and non-secure code

                    3. Privilege Protection:
                       - Controls privileged/unprivileged access
                       - Used for OS/application separation
                       - Enhances system security

                    Note: Only lower 16 bits of protection registers are used,
                    corresponding to the 16 sectors of DBANK.
                    `,
                        collapsed: true,
                        config: [
                            {
                                name: "dbankWriteEraseProtectSectors",
                                displayName: "Write/Erase Protected Sectors",
                                description: "Select sectors to be write/erase protected",
                                longDescription: `
                    Select sectors to enable write/erase protection:
                    - Protected sectors cannot be written or erased
                    - Each sector is 2KB
                    - Total of 16 sectors available
                    - Protection is permanent once set
                    `,
                                default: [],
                                options: DBANK_SECTORS,
                                minSelections: 0,
                                onChange: (inst, ui) => {
                                    updateDBANKWriteEraseProtection(inst, ui);
                                }
                            },
                            {
                                name: "dbankWriteEraseProtection",
                                displayName: "Write/Erase Protection Value",
                                description: "Generated protection value (only lower 16 bits used)",
                                displayFormat: "hex",
                                default: 0xFFFF,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "dbankSecurityProtectSectors",
                                displayName: "Security Protected Sectors",
                                description: "Select sectors to be security protected (secure)",
                                longDescription: `
                    Security Protection Configuration:
                    - Each bit represents a sector (16 bits total)
                    - Bit value 0: Sector is SECURE
                    - Bit value 1: Sector is NON-SECURE
                    - Only lower 16 bits are used
                    `,
                                default: [],
                                minSelections: 0,
                                options: DBANK_SECTORS,
                                onChange: (inst, ui) => {
                                    updateDBANKSecurityProtection(inst, ui);
                                }
                            },
                            {
                                name: "dbankSecurityProtection",
                                displayFormat: "hex",
                                default: 0x0000,
                                readOnly: true,
                                hidden: true
                            },
                            {
                                name: "dbankPrivilegeProtectSectors",
                                displayName: "Privilege Protected Sectors",
                                description: "Select sectors to be privilege protected",
                                longDescription: `
                    Privilege Protection Configuration:
                    - Each bit represents a sector (16 bits total)
                    - Bit value 0: Sector is PRIVILEGED
                    - Bit value 1: Sector is NON-PRIVILEGED
                    - Only lower 16 bits are used
                    `,
                                default: [],
                                minSelections: 0,
                                options: DBANK_SECTORS,
                                onChange: (inst, ui) => {
                                    updateDBANKPrivilegeProtection(inst, ui);
                                }
                            },
                            {
                                name: "dbankPrivilegeProtection",
                                displayFormat: "hex",
                                default: 0x0000,
                                readOnly: true,
                                hidden: true
                            }
                        ]
                    }
                ]
            },
            {
                name: "GROUP_SYSPLL_CONFIG",
                displayName: "SYSPLL Configuration",
                description: "System PLL Configuration",
                collapsed: true,
                config: [
                    {
                        name: "syspllConfig",
                        displayName: "SYSPLL Configuration",
                        description: "System PLL Configuration Mode",
                        longDescription: `
Configures the System PLL operation mode:
- NOPLL: PLL is not used
- PLL_80: PLL is configured for 80MHz operation
- PLL_CUSTOM: PLL uses custom configuration`,
                        default: "0xFF",
                        options: [
                            { name: "0xFF", displayName: "NOPLL" },
                            { name: "0xAA", displayName: "PLL_80" },
                            { name: "0xBB", displayName: "PLL_CUSTOM" }
                        ],
                        onChange: onChangePLL,
                    },
                    {
                        name: "GROUP_PLL_CUSTOM_CONFIG",
                        displayName: "PLL Custom Configuration",
                        description: "Custom PLL Configuration Options",
                        collapsed: false,
                        config: [
                            {
                                name: "syspllConfigQDiv",
                                displayName: "SYSPLL Q Divider",
                                description: "System PLL Q Divider value",
                                default: 0x0F,
                                readOnly: true,
                                range: [0, 0xFF]
                            },
                            {
                                name: "syspllConfigRDiv",
                                displayName: "SYSPLL R Divider",
                                description: "System PLL R Divider value",
                                default: 0x00,
                                readOnly: true,
                                range: [0, 0xFF]
                            }
                        ]
                    },
                    {
                        name: "GROUP_PLL_COMMON_CONFIG",
                        displayName: "PLL Common Configuration",
                        description: "Common PLL Configuration Options",
                        collapsed: false,
                        config: [
                            {
                                name: "syspllSource",
                                displayName: "HFCLK Source Configuration",
                                description: "Configures the High-Frequency Clock Source",
                                longDescription: `
                            Configures the high-frequency clock source which can be used:
                            1. As input to the System PLL when PLL is enabled
                            2. Directly as HFCLK when PLL is disabled

                            - DEFAULT (0xFF): Use SYSOSC (16MHz)
                            - HFXT_20MHZ (0xAA): Use 20MHz HFXT crystal
                            - HFXT_40MHZ (0xBB): Use 40MHz HFXT crystal

                            This configuration affects:
                            - PLL input source when PLL is enabled
                            - Direct HFCLK source when PLL is disabled
                            - MCAN clock source configuration
                            `,
                                default: "0xFF",
                                options: [
                                    { name: "0xFF", displayName: "SYSOSC (32MHz)" },
                                    { name: "0xAA", displayName: "HFXT (20MHz)" },
                                    { name: "0xBB", displayName: "HFXT (40MHz)" }
                                ]
                            },
                            {
                                name: "syspllSettlingTime",
                                displayName: "SYSPLL Settling Time",
                                description: "System PLL settling time in microseconds",
                                default: 0xC8,
                                range: [0, 0xFF]
                            },
                            {
                                name: "hfxtStartupMonitorTime",
                                displayName: "HFXT Startup Monitor Time",
                                description: "HFXT crystal oscillator startup monitoring time in microseconds",
                                default: 0x08,
                                range: [0, 0xFF]
                            }
                        ]
                    }
                ]
            },
            {
                name: "GROUP_BAD_CONV_PW",
                displayName: "Bad Device Conversion",
                description: "Configuration for device decommissioning service",
                longDescription: `
            Device Decommissioning Service Configuration

            This group configures the password required for the device decommissioning service,
            which allows transitioning the device from HS-FS to a decommissioned
            (DECOM) state.

            Key Points:
            1. The decommissioning process is IRREVERSIBLE
            2. The password must be stored as a SHA-256 hash value
            3. The hash is split into 8 32-bit words
            4. Once the device is decommissioned, it cannot be restored to functional state

            Password Requirements:
            - Must provide all 8 32-bit words of the SHA-256 hash
            - Password is required for the DSSM_COMMAND_CONVERT_BAD_DEVICE command
            - Command transitions device from HS-FS to DECOM state
            - Incorrect password will prevent the decommissioning process

            Security Considerations:
            - Store the password securely as it's required for decommissioning
            - Loss of password will prevent intended decommissioning
            - Ensure password is properly documented for authorized personnel

            Process:
            **Note**: Password fields for this device **require** the user to provide the SHA256
            hash of the 128-bit password. Refer to the following steps on correctly converting
            between 128-bit plaintext password and hash:

            1. Determine the 128-bit password to use. These steps will use the following
            example password:

            0x12345678\n
            0xCAFECAFE\n
            0xDEADBEEF\n
            0xCAFEBABE\n

            2. Reverse the password endianness. Combine it into one string.
            0x78563412\n
            0xFECAFECA\n
            0xEFBEADDE\n
            0xBEBAFECA\n

            Combined String: 78563412FECAFECAEFBEADDEBEBAFECA

            3. Calculate the SHA256 of the string. Break the output into 8 32-bit words.
            The user can use online tools like the one provided here:
            [SHA256 online tool](https://emn178.github.io/online-tools/sha256.html)

            Output String: 2738a5682ad52550177227468dd2b55f552c34bd25560b9067b0a3f3d5c648f9

            Result:\n
            0x2738a568\n
            0x2ad52550\n
            0x17722746\n
            0x8dd2b55f\n
            0x552c34bd\n
            0x25560b90\n
            0x67b0a3f3\n
            0xd5c648f9\n

            4. Reverse endianness of the 32-bit words.

            Original   -> Converted\n
            0x2738a568 -> 0x68A53827\n
            0x2ad52550 -> 0x5025D52A\n
            0x17722746 -> 0x46277217\n
            0x8dd2b55f -> 0x5FB5D28D\n
            0x552c34bd -> 0xBD342C55\n
            0x25560b90 -> 0x900B5625\n
            0x67b0a3f3 -> 0xF3A3B067\n
            0xd5c648f9 -> 0xF948C6D5\n

            5. Enter the converted password into the BCR (boot configuration routine) structure

            WARNING: This is a critical security feature. Improper configuration can
            permanently affect device functionality. Ensure all stakeholders understand
            the implications before proceeding.`,
                collapsed: true,
                config: badConvPWConfig,
            },
            {
                name: "GROUP_APP_CRC_CHECK_CONFIG",
                displayName: "Application Authentication Configuration",
                description: "Configure application authentication method",
                longDescription: `
                            IMPORTANT DEVICE STATE CONSIDERATIONS:
                            ------------------------------------
                            These authentication settings are only effective for devices in High Security - Field
                            Secured (HS-FS) state. For other device states:

                            * HS-SE (Security Enforced): These settings are ignored as the device uses its own
                            security mechanisms
                            * HS-KP (Key Provisioned): These settings are ignored as the device uses provisioned
                            key-based security

                            Note: Device state cannot be detected by the configuration tool as it is stored in
                            SOC flash. Ensure you know your device state before configuring these options.

                            Authentication Methods Available:
                            - Disabled: No authentication check
                            - CRC Check: Verify application integrity using CRC-32
                            - Hash Check: Verify application integrity using SHA-256

                            WARNING: Configuring these settings on non-HS-FS devices will not cause any errors
                            but the settings will have no effect.`,
                config: [
                    {
                        name: "secureBootMode",
                        displayName: "Application Authentication Mode",
                        description: "Controls application authentication method",
                        longDescription: `
                Select the authentication method for the application:
                - Disabled: No authentication check (default)
                - CRC Check: Verify application integrity using CRC-32
                - Hash Check: Verify application integrity using SHA-256`,
                        default: "disabled",
                        options: [
                            { name: "disabled", displayName: "Disabled" },
                            { name: "crcCheck", displayName: "CRC Check" },
                            { name: "hashCheck", displayName: "Hash Check" }
                        ],
                        onChange: (inst, ui) => {
                            // Update visibility of related fields
                            const showFields = inst.secureBootMode !== "disabled";
                            ui.appAuthStartAddress.hidden = !showFields;
                            ui.appAuthLength.hidden = !showFields;

                            // Show appropriate authentication value fields
                            ui.appAuthCRCValue.hidden = inst.secureBootMode !== "crcCheck";
                            ui.appAuthHashValue0.hidden = inst.secureBootMode !== "hashCheck";
                            ui.appAuthHashValue1.hidden = inst.secureBootMode !== "hashCheck";
                            ui.appAuthHashValue2.hidden = inst.secureBootMode !== "hashCheck";
                            ui.appAuthHashValue3.hidden = inst.secureBootMode !== "hashCheck";
                            ui.appAuthHashValue4.hidden = inst.secureBootMode !== "hashCheck";
                            ui.appAuthHashValue5.hidden = inst.secureBootMode !== "hashCheck";
                            ui.appAuthHashValue6.hidden = inst.secureBootMode !== "hashCheck";
                            ui.appAuthHashValue7.hidden = inst.secureBootMode !== "hashCheck";
                        }
                    },
                    {
                        name: "appAuthStartAddress",
                        displayName: "Authentication Start Address",
                        description: "The starting address for application authentication (must be in MAIN flash region)",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    },
                    {
                        name: "appAuthLength",
                        displayName: "Authentication Region Length",
                        description: "The length of the application region to authenticate",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    },
                    {
                        name: "appAuthCRCValue",
                        displayName: "Expected CRC-32 Value",
                        description: "The expected CRC-32 value to compare during boot",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    },
                    {
                        name: "appAuthHashValue0",
                        displayName: "Expected SHA-256 Hash [0]",
                        description: "Word 0 of the expected SHA-256 hash",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    },
                    {
                        name: "appAuthHashValue1",
                        displayName: "Expected SHA-256 Hash [1]",
                        description: "Word 1 of the expected SHA-256 hash",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    },
                    {
                        name: "appAuthHashValue2",
                        displayName: "Expected SHA-256 Hash [2]",
                        description: "Word 2 of the expected SHA-256 hash",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    },
                    {
                        name: "appAuthHashValue3",
                        displayName: "Expected SHA-256 Hash [3]",
                        description: "Word 3 of the expected SHA-256 hash",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    },
                    {
                        name: "appAuthHashValue4",
                        displayName: "Expected SHA-256 Hash [4]",
                        description: "Word 4 of the expected SHA-256 hash",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    },
                    {
                        name: "appAuthHashValue5",
                        displayName: "Expected SHA-256 Hash [5]",
                        description: "Word 5 of the expected SHA-256 hash",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    },
                    {
                        name: "appAuthHashValue6",
                        displayName: "Expected SHA-256 Hash [6]",
                        description: "Word 6 of the expected SHA-256 hash",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    },
                    {
                        name: "appAuthHashValue7",
                        displayName: "Expected SHA-256 Hash [7]",
                        description: "Word 7 of the expected SHA-256 hash",
                        hidden: true,
                        displayFormat: "hex",
                        default: 0xFFFFFFFF,
                        range: [0, 0xFFFFFFFF]
                    }
                ]
            }
        ].concat(extendedConfigBCR).concat([
            {
                // .fastBootMode
                /* BOOTCFG1.FASTBOOTMODE */
                name: "fastBootMode",
                displayName: "Enable Fast Boot Mode",
                description: "Enable Fast Boot Mode",
                longDescription: warningNoteM0C + `
The execution time of the BCR can be reduced by enabling fast boot mode.Fast
boot mode speeds up the boot process via the following methods:

* Limiting the BSL entry methods.Only SYSCTL register invoke and the DSSM
  invoke methods may be used to enter the bootloader.Other BSL invoke conditions
            such as pin - based invoke are not tested.

* Bypassing the application CRC check, even if the application CRC check is
            enabled.
`,
                hidden: false,
                default: false,
                readOnly: Common.isDeviceM0C(),
            },
            // .bcrConfigID
            {
                name: "bcrConfigID",
                displayName: "BCR Configuration ID",
                description: "Predetermined Config Signature Magic ID",
                longDescription: ``,
                hidden: false,
                readOnly: true,
                displayFormat: "hex",
                default: 0x05000000,
            },
            {
                /* BOOTCRC */
                name: "bootCRC",
                displayName: "Expected BCR Configuration CRC",
                description: "The expected CRC value to test against during boot",
                longDescription: warningNoteM0C + `
The BCR configuration data structure is protected by a 32 - bit CRC to improve
            security.The BCR configuration CRC is stored in the BOOTCRC field of NONMAIN.

If the stored CRC of the BCR configuration doesn't match the calculated CRC
during boot, the result is a boot error, and the device assumes a
                ** fully restrictive ** state.In this state, commands sent to the debug
mailbox are ignored, the BSL and the main application aren't started, and **it's
not possible to recover the device **.
`,
                hidden: false,
                displayFormat: "hex",
                default: 0,
                getValue: (inst) => {
                    return calculateBCRCRC(inst);
                },
            },
            {
                // .bootloaderMode
                /* BOOTCFG1.BSLMODE */
                name: "bslMode",
                displayName: "Enable BSL",
                description: "Enable BSL",
                longDescription: warningNoteM0C + `
                ** Note **, when the BSL is disabled, it is not possible to enter the BSL through
any invocation mechanism.
`,
                hidden: false,
                default: !(Common.isDeviceM0C()),
                readOnly: Common.isDeviceM0C(),
                onChange: onChangeBSLEnable,
            },
        ])
    },
]

/* JS CRC implementation from https://github.com/SheetJS/js-crc32 */
const CRC32 = require('./crc/cjs/calculators/crc32').default;

/*
 * Splits a string into substrings of a particular length. Helper function for
 * CRC calculations
 */
function chunkString(str, length) {
    return str.match(new RegExp('.{1,' + length + '}', 'g'));
}

function createBCRString(inst) {
    let bcrConfigStr = "";

    // 0x80101800-0x80101810
    bcrConfigStr += "00000005";

    // DEBUG_ACCESS and SWDP_MODE
    let debugAccessStr = "FFFF";
    if (inst.apEnable === "enabled") {
        debugAccessStr = "BBAA";  // Level 0 - Full Access
    } else if (inst.apEnable === "level1") {
        debugAccessStr = "DDCC";  // Level 1 - Secure Protected, Non-Secure Enabled
    } else if (inst.apEnable === "level2") {
        debugAccessStr = "CCDD";  // Level 2 - Both Secure and Non-Secure Protected
    } else if (inst.apEnable === "disabled") {
        debugAccessStr = "2255";  // Disabled
    }
    bcrConfigStr += debugAccessStr;

    // SWDP_MODE
    bcrConfigStr += (inst.swdEnable) ? "BBAA" : "2255";

    // BSL_PIN_INVOKE
    bcrConfigStr += (inst.bslInvokePinCheck) ? "BBAA" : "2255";

    // DEBUG_HOLD
    bcrConfigStr += (inst.debugHold) ? "2255" : "BBAA";

    // CSC_EXISTS and FLASH_BANK_SWAP_POLICY
    bcrConfigStr += (inst.cscExists) ? "2255" : "BBAA";
    bcrConfigStr += (inst.flashBankSwap) ? "FFFF" : "BBAA";

    // FAST_BOOT_MODE (0x80101810)
    bcrConfigStr += inst.fastBootMode ? "BBAA" : "FFFF";

    // BOOTLOADER_MODE (0x80101810 high 16 bits)
    bcrConfigStr += inst.bslMode ? "BBAA" : "2255";

    // MASS_ERASE_MODE and FACTORY_RESET_MODE
    bcrConfigStr += (inst.meMode === "enabled" ? "BBAA" : inst.meMode === "enabledWithPW" ? "DDCC" : "2255") +
        (inst.frMode === "enabled" ? "BBAA" : inst.frMode === "enabledWithPW" ? "DDCC" : "2255");

    // Mass Erase Password
    if (inst.meMode === "enabledWithPW") {
        for (let i = 0; i < 8; i++) {
            let pwValue = inst[`mePW${i}`].toString(16).padStart(8, '0');
            bcrConfigStr += pwValue.match(/.{2}/g).reverse().join('');
        }
    } else {
        for (let i = 0; i < 8; i++) {
            bcrConfigStr += "FFFFFFFF";
        }
    }

    // Factory Reset Password
    if (inst.frMode === "enabledWithPW") {
        for (let i = 0; i < 8; i++) {
            let pwValue = inst[`frPW${i}`].toString(16).padStart(8, '0');
            bcrConfigStr += pwValue.match(/.{2}/g).reverse().join('');
        }
    } else {
        for (let i = 0; i < 8; i++) {
            bcrConfigStr += "FFFFFFFF";
        }
    }

    // Debug Lock Password
    if (inst.apEnable === "level1" || inst.apEnable === "level2") {
        for (let i = 0; i < 8; i++) {
            let pwValue = inst[`dbglockPW${i}`].toString(16).padStart(8, '0');
            bcrConfigStr += pwValue.match(/.{2}/g).reverse().join('');
        }
    } else {
        for (let i = 0; i < 8; i++) {
            bcrConfigStr += "FFFFFFFF";
        }
    }

    // Debug NS Lock Password
    if (inst.apEnable === "level2") {
        for (let i = 0; i < 8; i++) {
            let pwValue = inst[`dbgnslockPW${i}`].toString(16).padStart(8, '0');
            bcrConfigStr += pwValue.match(/.{2}/g).reverse().join('');
        }
    } else {
        for (let i = 0; i < 8; i++) {
            bcrConfigStr += "FFFFFFFF";
        }
    }

    // Bad Conversion Password (0x80101898-0x801018B4)
    for (let i = 0; i < 8; i++) {
        let pwValue = inst[`badConvPW${i}`].toString(16).padStart(8, '0');
        bcrConfigStr += pwValue.match(/.{2}/g).reverse().join('');
    }

    // SECURE_BOOT_MODE (0x801018B8)
    let secureBootValue;
    switch (inst.secureBootMode) {
        case "crcCheck":
            secureBootValue = "BBAA"; // BC_CFG_SECURE_BOOT_CRC_EN
            break;
        case "hashCheck":
            secureBootValue = "DDCC"; // BC_CFG_SECURE_BOOT_HASH_EN
            break;
        default:
            secureBootValue = "FFFF"; // Disabled
    }
    bcrConfigStr += secureBootValue;

    // BC_RESERVED_0 (0x801018B8 high 16 bits)
    bcrConfigStr += "FFFF";

    // USER_SECURE_APP_START_ADDR (0x801018BC)
    // Converting address to 8-character hex string, maintaining little-endian format
    let startAddr = inst.appAuthStartAddress.toString(16).padStart(8, '0').toUpperCase();
    bcrConfigStr += startAddr;

    // USER_SECURE_APP_LENGTH (0x801018C0)
    // Converting length to 8-character hex string, maintaining little-endian format
    let appLength = inst.appAuthLength.toString(16).padStart(8, '0').toUpperCase();
    bcrConfigStr += appLength;

    // USER_SECURE_APP_HASH (0x801018C4-0x801018E0)
    // 8 32-bit words of hash values
    if (inst.secureBootMode === "hashCheck") {
        // Add all 8 words of the hash value
        for (let i = 0; i < 8; i++) {
            let hashValue = inst[`appAuthHashValue${i}`]
                .toString(16)
                .padStart(8, '0');
            bcrConfigStr += hashValue;
        }
    } else {
        // If hash check is not enabled, fill with 0xFFFFFFFF
        for (let i = 0; i < 8; i++) {
            bcrConfigStr += "FFFFFFFF";
        }
    }

    bcrConfigStr += (inst.nonMainWriteProtection === "disabled") ? "BBAA" : "2255";
    bcrConfigStr += "FFFF";  // BC_RESERVED_1
    // 0x801018E8-0x801018EC: BANK0_WRITE_ERASE_PROTECTION_A/B
    let bank0WepA = (inst.bank0WriteEraseProtectionA >>> 0).toString(16).padStart(8, '0');
    let bank0WepB = (inst.bank0WriteEraseProtectionB >>> 0).toString(16).padStart(8, '0');
    let bank0SecA = (inst.bank0SecurityProtectionA >>> 0).toString(16).padStart(8, '0');
    let bank0SecB = (inst.bank0SecurityProtectionB >>> 0).toString(16).padStart(8, '0');
    let bank0PrivA = (inst.bank0PrivilegeProtectionA >>> 0).toString(16).padStart(8, '0');
    let bank0PrivB = (inst.bank0PrivilegeProtectionB >>> 0).toString(16).padStart(8, '0');


    // Convert to little-endian format
    bcrConfigStr += bank0WepA.match(/.{2}/g).reverse().join('');  // BANK0_WRITE_ERASE_PROTECTION_A
    bcrConfigStr += bank0WepB.match(/.{2}/g).reverse().join('');  // BANK0_WRITE_ERASE_PROTECTION_B
    bcrConfigStr += bank0SecA.match(/.{2}/g).reverse().join('');
    bcrConfigStr += bank0SecB.match(/.{2}/g).reverse().join('');
    bcrConfigStr += bank0PrivA.match(/.{2}/g).reverse().join('');
    bcrConfigStr += bank0PrivB.match(/.{2}/g).reverse().join('');

    let bank1WepA = (inst.bank1WriteEraseProtectionA >>> 0).toString(16).padStart(8, '0');
    let bank1WepB = (inst.bank1WriteEraseProtectionB >>> 0).toString(16).padStart(8, '0');
    let bank1SecA = (inst.bank1SecurityProtectionA >>> 0).toString(16).padStart(8, '0');
    let bank1SecB = (inst.bank1SecurityProtectionB >>> 0).toString(16).padStart(8, '0');
    let bank1PrivA = (inst.bank1PrivilegeProtectionA >>> 0).toString(16).padStart(8, '0');
    let bank1PrivB = (inst.bank1PrivilegeProtectionB >>> 0).toString(16).padStart(8, '0');


    // 0x80101900-0x80101920
    bcrConfigStr += "FFFFFFFF";
    bcrConfigStr += bank1WepA.match(/.{2}/g).reverse().join('');
    bcrConfigStr += bank1WepB.match(/.{2}/g).reverse().join('');
    bcrConfigStr += bank1SecA.match(/.{2}/g).reverse().join('');
    bcrConfigStr += bank1SecB.match(/.{2}/g).reverse().join('');
    bcrConfigStr += bank1PrivA.match(/.{2}/g).reverse().join('');
    bcrConfigStr += bank1PrivB.match(/.{2}/g).reverse().join('');
    bcrConfigStr += "FFFFFFFF";

    // 0x80101920-0x80101930
    let dbankWEP = ((0xFFFF << 16) | (inst.dbankWriteEraseProtection & 0xFFFF)) >>> 0;
    bcrConfigStr += dbankWEP.toString(16).padStart(8, "0").match(/.{2}/g).reverse().join('');

    // DBANK Security Protection (upper 16 bits: 0x0000)
    let dbankSP = (0x0000 << 16) | (inst.dbankSecurityProtection & 0xFFFF);
    bcrConfigStr += dbankSP.toString(16).padStart(8, "0").match(/.{2}/g).reverse().join('');

    // DBANK Privilege Protection (upper 16 bits: 0x0000)
    let dbankPP = (0x0000 << 16) | (inst.dbankPrivilegeProtection & 0xFFFF);
    bcrConfigStr += dbankPP.toString(16).padStart(8, "0").match(/.{2}/g).reverse().join('');
    bcrConfigStr += "FFFFFFFF";

    // 0x80101930-0x80101940 (SYSPLL section)
    // First word (0x80101930): SYSPLL_CONFIG[7..0], QDIV[15..8], RDIV[23..16], SOURCE[31..24]
    let syspllWord = "";
    // Build in little-endian order
    syspllWord += inst.syspllConfig.replace("0x", "");  // [7..0] Config - Remove "0x" prefix
    syspllWord += inst.syspllConfigQDiv.toString(16).padStart(2, '0').toUpperCase();  // [15..8] QDIV
    syspllWord += inst.syspllConfigRDiv.toString(16).padStart(2, '0').toUpperCase();  // [23..16] RDIV
    syspllWord += inst.syspllSource.replace("0x", "");  // [31..24] Source - Remove "0x" prefix
    bcrConfigStr += syspllWord;

    // Second word (0x80101934): SETTLING_TIME[7..0], MONITOR_TIME[15..8], RESVD[31..16]
    let syspllTimingWord = "";
    // Build in little-endian order
    syspllTimingWord += inst.syspllSettlingTime.toString(16).padStart(2, '0').toUpperCase();    // [7..0]
    syspllTimingWord += inst.hfxtStartupMonitorTime.toString(16).padStart(2, '0').toUpperCase(); // [15..8]
    syspllTimingWord += "FFFF";  // [31..16] Reserved
    bcrConfigStr += syspllTimingWord;

    bcrConfigStr += "FFFFFFFF";
    bcrConfigStr += "FFFFFFFF";

    // 0x80101940-0x8010194C (excluding CRC)
    bcrConfigStr += "FFFFFFFF";
    bcrConfigStr += "FFFFFFFF";
    bcrConfigStr += "FFFFFFFF";

    return bcrConfigStr;
}

function calculateBCRCRC(inst) {
    let bcrConfigStr = createBCRString(inst);

    /* Convert into an array and reverse the values */
    let bcrConfigArr = chunkString(bcrConfigStr, 2);

    /* Convert array of strings into array of hex numbers */
    let crc = CRC32(bcrConfigArr.map(el => parseInt(el, 16)));

    /* Calculate the JAMCRC and get the unsigned value */
    crc = (~crc) >>> 0

    return (crc);
}

function isStandaloneNumberPresent(number, str) {
    const regex = new RegExp(`(?:^|,\s*)(${number})(?=\s*,|$)`);
    const match = str.match(regex);
    /* Match returns the match and captured groups, return the captured group */
    return match ? match[1] : null;
}

/* Calculate 32-bit CRCJAM over BSL config string (supporting ROM BSL) */
function calculateBSLCRC_32Bit(inst) {

    let bslConfigStr = createROMBSLString(inst);

    /* Convert into an array and reverse the values */
    let bslConfigArr = chunkString(bslConfigStr, 2);

    /* Convert array of strings into array of hex numbers */
    let crc = CRC32(bslConfigArr.map(el => parseInt(el, 16)));

    /* Calculate the JAMCRC and get the unsigned value */
    crc = (~crc) >>> 0

    return (crc);
}

function reverseBytes(hexString) {
    let result = '';
    for (let i = hexString.length - 2; i >= 0; i -= 2) {
        result += hexString.substr(i, 2);
    }
    return result;
}

function createROMBSLString(inst) {
    let bslConfigStr = "";

    // configID - Predetermined config signature magic ID
    let configId = (inst.bslConfigID).toString(16).padStart(8, "0");
    bslConfigStr += reverseBytes(configId);

    // interfacePins - BSL interface pins
    bslConfigStr += (inst.uartRXPadNum).toString(16).padStart(2, "0") +
        (inst.uartRXMux).toString(16).padStart(2, "0") +
        (inst.uartTXPadNum).toString(16).padStart(2, "0") +
        (inst.uartTXMux).toString(16).padStart(2, "0");

    // I2C Configuration
    bslConfigStr += (inst.i2cSDAPadNum).toString(16).padStart(2, "0") +
        (inst.i2cSDAMux).toString(16).padStart(2, "0") +
        (inst.i2cSCLPadNum).toString(16).padStart(2, "0") +
        (inst.i2cSCLMux).toString(16).padStart(2, "0")

    // MCAN Configuration (if supported)
    bslConfigStr += (inst.mcanRXPadNum).toString(16).padStart(2, "0") +
        (inst.mcanRXMux).toString(16).padStart(2, "0") +
        (inst.mcanTXPadNum).toString(16).padStart(2, "0") +
        (inst.mcanTXMux).toString(16).padStart(2, "0");

    // pin - BSL invocation pin
    bslConfigStr += (inst.bslInvokePinData0).toString(16).toUpperCase().padStart(2, "0") +
        (inst.bslInvokePinData1).toString(16).toUpperCase().padStart(2, "0");

    // memoryRead - Enable/disable memory readout
    bslConfigStr += (inst.bslEnableReadOut) ? "BBAA" : "FFFF";

    // password - 64-byte password (8 32-bit words)
    bslConfigStr += reverseBytes((inst.bslPW7).toString(16).toUpperCase().padStart(8, "0")) +
        reverseBytes((inst.bslPW6).toString(16).toUpperCase().padStart(8, "0")) +
        reverseBytes((inst.bslPW5).toString(16).toUpperCase().padStart(8, "0")) +
        reverseBytes((inst.bslPW4).toString(16).toUpperCase().padStart(8, "0")) +
        reverseBytes((inst.bslPW3).toString(16).toUpperCase().padStart(8, "0")) +
        reverseBytes((inst.bslPW2).toString(16).toUpperCase().padStart(8, "0")) +
        reverseBytes((inst.bslPW1).toString(16).toUpperCase().padStart(8, "0")) +
        reverseBytes((inst.bslPW0).toString(16).toUpperCase().padStart(8, "0"));
    // appRev - Pointer to version information
    bslConfigStr += (inst.bslAppVersion).toString(16).toUpperCase().padStart(8, "0");

    // securityAlert - Security alert configuration
    let securityAlertStr = "FFFF";
    if (inst.bslSecurityConfig === "triggerFactoryReset") {
        securityAlertStr = "BBAA";
    } else if (inst.bslSecurityConfig === "disableBSL") {
        securityAlertStr = "DDCC";
    }

    let baudRate = inst.uartBaudDefault;
    // Convert "0002" to "0200" by swapping the bytes
    let formattedBaudRate = baudRate.substr(2, 2) + baudRate.substr(0, 2);
    bslConfigStr += securityAlertStr + formattedBaudRate;


    let i2cAddrHex = (inst.i2cSlaveAddress).toString(16).padStart(4, '0');
    // Reverse bytes for little-endian format
    i2cAddrHex = i2cAddrHex.substr(2, 2) + i2cAddrHex.substr(0, 2);

    // i2cSlaveAddress - I2C slave address
    bslConfigStr += i2cAddrHex

    for (let i = 0; i < 14; i++) {
        bslConfigStr += "FF";
    }

    return bslConfigStr;
}

/* Get device data pins with a valid PINCM */
let validDeviceDataPins = _.filter(system.deviceData.devicePins, (pin) =>
    Common.getAttribute((pin), ("iomux_pincm")) != "None");
/* Create array of GPIO pin names */
const pinOptions = _.map(validDeviceDataPins, (pin) =>
    ({ name: pin.mux.muxSetting[0].peripheralPin.peripheralName }));

const filteredPinOptions = pinOptions.filter((pin) => { return Common.getGPIOPort(pin.name) });
/* Create array of GPIO pins with their associated PINCM */
const iomuxPincmOptions = _.map(validDeviceDataPins, (pin) =>
    ({ name: pin.designSignalName, num: Common.getAttribute((pin), ("iomux_pincm")) }));

/* Create array of pins with UART TX functionality */
let uartTXPinData = validDeviceDataPins.map((obj) => ({
    ...obj,
    muxOptions: obj.mux.muxSetting.filter((muxOption) => muxOption.peripheralPin.name.match(/UC\d+(_\d+)?\.TX$/)),
})).filter(pin => pin.muxOptions.length > 0);  // Only keep pins that have valid mux options

/* Create array of pins with UART RX functionality */
let uartRXPinData = validDeviceDataPins.map((obj) => ({
    ...obj,
    muxOptions: obj.mux.muxSetting.filter((muxOption) => muxOption.peripheralPin.name.match(/UC\d+(_\d+)?\.RX$/)),
})).filter(pin => pin.muxOptions.length > 0);  // Only keep pins that have valid mux options

/* Create array of pins with I2C SCL functionality */
let i2cSCLPinData = validDeviceDataPins.map((obj) => ({
    ...obj,
    muxOptions: obj.mux.muxSetting.filter((muxOption) => muxOption.peripheralPin.name.match(/UC\d+(_\d+)?\.SCL$/)),
}));
i2cSCLPinData = _.filter(i2cSCLPinData, (pin) => pin.muxOptions.length);

/* Create array of pins with I2C SDA functionality */
let i2cSDAPinData = validDeviceDataPins.map((obj) => ({
    ...obj,
    muxOptions: obj.mux.muxSetting.filter((muxOption) => muxOption.peripheralPin.name.match(/UC\d+(_\d+)?\.SDA$/)),
}));
i2cSDAPinData = _.filter(i2cSDAPinData, (pin) => pin.muxOptions.length);

/* Create array of pins with MCAN RX functionality */
let mcanRXPinData = validDeviceDataPins.map((obj) => ({
    ...obj,
    muxOptions: obj.mux?.muxSetting?.filter((muxOption) =>
        muxOption.peripheralPin?.name?.match(/CANFD\d+.CANRX$/)) || []
})).filter(pin => pin.muxOptions.length > 0);

let mcanTXPinData = validDeviceDataPins.map((obj) => ({
    ...obj,
    muxOptions: obj.mux?.muxSetting?.filter((muxOption) =>
        muxOption.peripheralPin?.name?.match(/CANFD\d+.CANTX$/)) || []
})).filter(pin => pin.muxOptions.length > 0);

let defaultBSLUARTRXPinCM;
let defaultBSLUARTTXPinCM;
let defaultBSLI2CSCLPinCM;
let defaultBSLI2CSDAPinCM;
let defaultBSLMCANRXPinCM;
let defaultBSLMCANTXPinCM

let defaultBSLUARTRX = "";
let defaultBSLUARTTX = "";
let defaultBSLI2CSCL = "";
let defaultBSLI2CSDA = "";
let defaultBSLMCANTX = "";
let defaultBSLMCANRX = "";

/* Get default BSL UART/I2C/MCAN pins (based on device). num field can be more
 * than one entry e.g. "A,B" because of dual-purpose pins */

/* Get default BSL UART/I2C/MCAN pins (based on device) */
defaultBSLUARTRX = iomuxPincmOptions.find(x => isStandaloneNumberPresent(deviceOptions.BSL_UART_RX_PINCM, x.num));
defaultBSLUARTTX = iomuxPincmOptions.find(x => isStandaloneNumberPresent(deviceOptions.BSL_UART_TX_PINCM, x.num));
defaultBSLI2CSCL = iomuxPincmOptions.find(x => isStandaloneNumberPresent(deviceOptions.BSL_I2C_SCL_PINCM, x.num));
defaultBSLI2CSDA = iomuxPincmOptions.find(x => isStandaloneNumberPresent(deviceOptions.BSL_I2C_SDA_PINCM, x.num));
defaultBSLMCANRX = iomuxPincmOptions.find(x => isStandaloneNumberPresent(deviceOptions.BSL_MCAN_RX_PINCM, x.num));
defaultBSLMCANTX = iomuxPincmOptions.find(x => isStandaloneNumberPresent(deviceOptions.BSL_MCAN_TX_PINCM, x.num));

defaultBSLUARTRX = (!defaultBSLUARTRX) ? "0xFF" : defaultBSLUARTRX.name;
defaultBSLUARTTX = (!defaultBSLUARTTX) ? "0xFF" : defaultBSLUARTTX.name;
defaultBSLI2CSCL = (!defaultBSLI2CSCL) ? "0xFF" : defaultBSLI2CSCL.name;
defaultBSLI2CSDA = (!defaultBSLI2CSDA) ? "0xFF" : defaultBSLI2CSDA.name;
defaultBSLMCANRX = (!defaultBSLMCANRX) ? "0xFF" : defaultBSLMCANRX.name;
defaultBSLMCANTX = (!defaultBSLMCANTX) ? "0xFF" : defaultBSLMCANTX.name;

let bslExtendedConfigUART = [];
if (deviceOptions.BSL_SUPPORT_UART_BAUD == true) {
    bslExtendedConfigUART = [
        {
            name: "uartBaudDefault",
            displayName: "Default UART Baud Rate",
            default: "0002",
            options: [
                { name: "0001", displayName: "4800" },
                { name: "0002", displayName: "9600" },
                { name: "0003", displayName: "19200" },
                { name: "0004", displayName: "38400" },
                { name: "0005", displayName: "576000" },
                { name: "0006", displayName: "115200" },
                { name: "0007", displayName: "1000000" },
                { name: "0008", displayName: "2000000" },
                { name: "0009", displayName: "3000000" },
            ]
        }
    ]
}

let extendedConfigROMBSL = []
extendedConfigROMBSL = [
    {
        name: "GROUP_BSL_UART_PIN_CONFIG",
        displayName: "BSL UART Pin Configuration",
        description: "",
        config: [
            {
                name: "uartPeripheral",
                displayName: "UART Peripheral",
                description: "Select the UART peripheral",
                hidden: false,
                /* BSL UART instance is tied to UC12 */
                readOnly: true,
                default: "UC12",
                getValue: (inst) => {
                    /* Peripheral selection is tied to a specific
                       instance, dependent on selected device */
                    return deviceOptions.BSL_UART_INSTANCE;
                }
            },
        ].concat(bslExtendedConfigUART).concat([
            {
                name: "uartTXPin",
                displayName: "UART TX Pin",
                description: "Select the UART TX pin",
                hidden: false,
                default: defaultBSLUARTTX,
                options: createUARTTXPinOptions,
            },
            {
                // .interfacePins.UART_TXD_pad_num
                name: "uartTXPadNum",
                displayName: "UART TX Pad Number",
                description: "Pad number represents the IOMUX PINCM value",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = uartTXPinData.find(pin => pin.designSignalName == inst.uartTXPin);
                    let iomuxPINCM = 0;

                    if (selectedPin) {
                        let pinCM = Common.getPinCM(selectedPin.name, "TX", inst.uartPeripheral);
                        if (pinCM == undefined) { return -1 };
                        /* Convert to integer with radix 10 */
                        iomuxPINCM = parseInt(pinCM, 10);
                    }

                    return iomuxPINCM;
                }
            },
            {
                // .interfacePins.UART_TXD_PF_mux_sel
                name: "uartTXMux",
                displayName: "UART TX Mux",
                description: "The mux value represents the pin function",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = uartTXPinData.find(pin => pin.designSignalName == inst.uartTXPin);
                    let muxSetting = 0;

                    if (selectedPin) {
                        try {
                            muxSetting = parseInt(selectedPin.muxOptions.filter(a => a.peripheralPin.name == inst.uartPeripheral + ".TX")[0].mode, 10);
                        } catch (e) { return -1 };
                    }

                    return muxSetting;
                }
            },
            {
                name: "uartRXPin",
                displayName: "UART RX Pin",
                description: "Select the UART RX pin",
                hidden: false,
                default: defaultBSLUARTRX,
                options: createUARTRXPinOptions,
            },
            {
                // .interfacePins.UART_RXD_pad_num
                name: "uartRXPadNum",
                displayName: "UART RX Pad Number",
                description: "Pad number represents the IOMUX PINCM value",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = uartRXPinData.find(pin => pin.designSignalName == inst.uartRXPin);
                    let iomuxPINCM = 0;

                    if (selectedPin) {
                        let pinCM = Common.getPinCM(selectedPin.name, "RX", inst.uartPeripheral);
                        if (pinCM == undefined) { return -1 };
                        /* Convert to integer with radix 10 */
                        iomuxPINCM = parseInt(pinCM, 10);
                    }

                    return iomuxPINCM;
                }
            },
            {
                // .interfacePins.UART_RXD_PF_mux_sel
                name: "uartRXMux",
                displayName: "UART RX Mux",
                description: "The mux value represents the pin function",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = uartRXPinData.find(pin => pin.designSignalName == inst.uartRXPin);
                    let muxSetting = 0;

                    if (selectedPin) {
                        try {
                            muxSetting = parseInt(selectedPin.muxOptions.filter(a => a.peripheralPin.name == inst.uartPeripheral + ".RX")[0].mode, 10);
                        } catch (e) { return -1 };
                    }

                    return muxSetting;
                }
            },
        ],)
    },
    {
        name: "GROUP_BSL_I2C_PIN_CONFIG",
        displayName: "BSL I2C Pin Configuration",
        description: "",
        config: [
            {
                name: "i2cPeripheral",
                displayName: "I2C Peripheral",
                description: "Select the I2C peripheral",
                hidden: false,
                /* BSL I2C instance is tied to UC15_0 */
                readOnly: true,
                default: "UC15_0",
                getValue: (inst) => {
                    return deviceOptions.BSL_I2C_INSTANCE;
                }
            },
            {
                name: "i2cSCLPin",
                displayName: "I2C SCL Pin",
                description: "Select the I2C SCL pin",
                hidden: false,
                default: defaultBSLI2CSCL,
                options: createI2CSCLPinOptions,
            },
            {
                // .interfacePins.I2C_SCL_pad_num
                name: "i2cSCLPadNum",
                displayName: "I2C SCL Pad Number",
                description: "Pad number represents the IOMUX PINCM value",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = i2cSCLPinData.find(pin => pin.designSignalName == inst.i2cSCLPin);
                    let iomuxPINCM = 0;

                    if (selectedPin) {
                        let pinCM = Common.getPinCM(selectedPin.name, "SCL", inst.i2cPeripheral);
                        if (pinCM == undefined) { return -1 };
                        /* Convert to integer with radix 10 */
                        iomuxPINCM = parseInt(pinCM, 10);
                    }

                    return iomuxPINCM;
                }
            },
            {
                // .interfacePins.I2C_SCL_PF_mux_sel
                name: "i2cSCLMux",
                displayName: "I2C SCL Mux",
                description: "The mux value represents the pin function",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = i2cSCLPinData.find(pin => pin.designSignalName == inst.i2cSCLPin);
                    let muxSetting = 0;

                    if (selectedPin) {
                        try {
                            muxSetting = parseInt(selectedPin.muxOptions.filter(a => a.peripheralPin.name == inst.i2cPeripheral + ".SCL")[0].mode, 10);
                        } catch (e) { return -1 };
                    }

                    return muxSetting;
                }
            },
            {
                name: "i2cSDAPin",
                displayName: "I2C SDA Pin",
                description: "Select the I2C SDA pin",
                hidden: false,
                default: defaultBSLI2CSDA,
                options: createI2CSDAPinOptions,
            },
            {
                // .interfacePins.I2C_SDA_pad_num
                name: "i2cSDAPadNum",
                displayName: "I2C SDA Pad Number",
                description: "Pad number represents the IOMUX PINCM value",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = i2cSDAPinData.find(pin => pin.designSignalName == inst.i2cSDAPin);
                    let iomuxPINCM = 0;

                    if (selectedPin) {
                        let pinCM = Common.getPinCM(selectedPin.name, "SDA", inst.i2cPeripheral);
                        if (pinCM == undefined) { return -1 };
                        /* Convert to integer with radix 10 */
                        iomuxPINCM = parseInt(pinCM, 10);
                    }

                    return iomuxPINCM;
                }
            },
            {
                // .interfacePins.I2C_SDA_PF_mux_sel
                name: "i2cSDAMux",
                displayName: "I2C SDA Mux",
                description: "The mux value represents the pin function",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = i2cSDAPinData.find(pin => pin.designSignalName == inst.i2cSDAPin);
                    let muxSetting = 0;

                    if (selectedPin) {
                        try {
                            muxSetting = parseInt(selectedPin.muxOptions.filter(a => a.peripheralPin.name == inst.i2cPeripheral + ".SDA")[0].mode, 10);
                        } catch (e) { return -1 }
                    }

                    return muxSetting;
                }
            },
            {
                // .i2cSlaveAddress
                name: "i2cSlaveAddress",
                displayName: "I2C Target Address (7-bit)",
                default: 0x0048,
                displayFormat: "hex",
                range: [0, 0x007F],
            },
        ],
    },
    {
        name: "GROUP_BSL_MCAN_PIN_CONFIG",
        displayName: "BSL MCAN Pin Configuration",
        description: "",
        config: [
            {
                name: "mcanPeripheral",
                displayName: "MCAN Peripheral",
                description: "Select the MCAN peripheral",
                hidden: false,
                readOnly: true,
                default: "CANFD0",
                getValue: (inst) => {
                    return deviceOptions.BSL_MCAN_INSTANCE;
                }
            },
            {
                name: "mcanTXPin",
                displayName: "MCAN TX Pin",
                description: "Select the MCAN TX pin",
                hidden: false,
                default: defaultBSLMCANTX,
                options: createMCANTXPinOptions,
            },
            {
                // .interfacePins.MCAN_TXD_pad_num
                name: "mcanTXPadNum",
                displayName: "MCAN TX Pad Number",
                description: "Pad number represents the IOMUX PINCM value",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = mcanTXPinData.find(pin => pin.designSignalName == inst.mcanTXPin);
                    let iomuxPINCM = 0;

                    if (selectedPin) {
                        /* Convert to integer with radix 10 */
                        iomuxPINCM = parseInt(selectedPin.attributes.iomux_pincm, 10);
                    }

                    return iomuxPINCM;
                }
            },
            {
                // .interfacePins.MCAN_TXD_PF_mux_sel
                name: "mcanTXMux",
                displayName: "MCAN TX Mux",
                description: "The mux value represents the pin function",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = mcanTXPinData.find(pin => pin.designSignalName == inst.mcanTXPin);
                    let muxSetting = 0;

                    if (selectedPin) {
                        muxSetting = parseInt(selectedPin.muxOptions[0].mode, 10);
                    }

                    return muxSetting;
                }
            },
            {
                name: "mcanRXPin",
                displayName: "MCAN RX Pin",
                description: "Select the MCAN RX pin",
                hidden: false,
                default: defaultBSLMCANRX,
                options: createMCANRXPinOptions,
            },
            {
                // .interfacePins.MCAN_RXD_pad_num
                name: "mcanRXPadNum",
                displayName: "MCAN RX Pad Number",
                description: "Pad number represents the IOMUX PINCM value",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = mcanRXPinData.find(pin => pin.designSignalName == inst.mcanRXPin);
                    let iomuxPINCM = 0;

                    if (selectedPin) {
                        /* Convert to integer with radix 10 */
                        iomuxPINCM = parseInt(selectedPin.attributes.iomux_pincm, 10);
                    }

                    return iomuxPINCM;
                }
            },
            {
                // .interfacePins.MCAN_RXD_PF_mux_sel
                name: "mcanRXMux",
                displayName: "MCAN RX Mux",
                description: "The mux value represents the pin function",
                default: 0,
                getValue: (inst) => {
                    let selectedPin = mcanRXPinData.find(pin => pin.designSignalName == inst.mcanRXPin);
                    let muxSetting = 0;

                    if (selectedPin) {
                        muxSetting = parseInt(selectedPin.muxOptions[0].mode, 10);
                    }

                    return muxSetting;
                }
            }
        ]
    },
];


nonMainConfig = nonMainConfig.concat(([
    /****** BSL CONFIGURATION *******/
    {
        name: "GROUP_BSL",
        displayName: "Bootstrap Loader (BSL) Configuration",
        description: "",
        longDescription: `
    The BSL provides a method to program and / or verify the device memory through a
    standard serial interface(UART, I2C, or MCAN).
    Key features of the BSL which are accessible through the serial interface include:

    * Programming and erase of flash memory
                * Ability to return a firmware version number through a pointer to the MAIN flash
                    * Ability to specify a hardware invoke GPIO
                        * Ability to enable code / data read - out(disabled by default)
    * Ability to return a 32 - bit CRC of a code / data region(1kB minimum region size)
    to verify programming
                * Access is always protected with a 256 - bit password
                    * Configurable security alert handling for resisting brute force attacks
                        * Support for MAIN flash plugins to enable additional interfaces beyond UART, I2C, and MCAN

    For a complete description of the BSL features, see the BSL User's Guide.

    The BSL can be completely disabled if desired by properly configuring the BSL
            mode in the BCR configuration in NONMAIN flash.
    `,
        collapsed: true,
        config: [
            {
                // .password
                /* BSLPW[0-7] */
                name: "GROUP_BSL_ACCESS_PW",
                displayName: "BSL Access Password",
                longDescription: sha256NoteAlt,
                config: createPWConfig("bslPW", false, 8),
            },
            {
                name: "GROUP_BSL_GPIO_INVOKE_CONFIG",
                displayName: "BSL GPIO Invoke Configuration",
                description: "",
                config: [
                    {
                        // .bslPinInvokeEnable
                        /* BOOTCFG0.BSLIVKPINEN */
                        name: "bslInvokePinCheck",
                        displayName: "Enable BSL Invoke Pin Check",
                        description: "Enable BSL Invoke Pin Check",
                        longDescription: ``,
                        hidden: false,
                        default: true,
                        onChange: updateGUIGPIOInvoke,
                    },
                    {
                        name: "bslDefaultInvokePin",
                        displayName: "Use Default BSL Invoke Pin",
                        description: "Use Default BSL Invoke Pin",
                        longDescription: `
    The default BSL invoke pin on M0L and M0G devices is PA18.Refer to the device
    datasheet for more details.
            `,
                        hidden: false,
                        default: true,
                        onChange: updateGUIGPIOInvoke,
                    },
                    {
                        /* BSLCFG.BSLIVK_GPIOPORT. BSLCFG.BSLIVK_GPIOPIN */
                        name: "bslInvokePin",
                        displayName: "BSL Invoke Pin",
                        description: "Select BSL Invoke Pin",
                        longDescription: ``,
                        hidden: true,
                        default: filteredPinOptions[0].name,
                        options: filteredPinOptions,
                    },
                    {
                        /* BSLCFG.BSLIVK_PINCM */
                        name: "bslInvokePINCM",
                        displayName: "BSL Invoke Pin PINCM",
                        description: "BSL Invoke Pin PINCM",
                        longDescription: ``,
                        hidden: true,
                        readOnly: true,
                        default: "1",
                        getValue: (inst) => {
                            return iomuxPincmOptions.find(x => x.name.includes(inst.bslInvokePin)).num;
                        }
                    },
                    {
                        /* BSLCFG.BSLIVK_LVL */
                        name: "bslInvokePinLevel",
                        displayName: "BSL Invoke Pin Level",
                        description: "BSL Invoke Pin Level",
                        longDescription: ``,
                        hidden: true,
                        default: 0,
                        options: [
                            { name: 0, displayName: "Low" },
                            { name: 1, displayName: "High" },
                        ],
                    },
                    {
                        // .pin.pinData0
                        /* For background calculation only */
                        name: "bslInvokePinData0",
                        displayName: "BSL Invoke Pin Details: Data0",
                        description: "BSL Invoke Pin Details: Data0",
                        longDescription: ``,
                        hidden: true,
                        readOnly: true,
                        displayFormat: "hex",
                        default: 0,
                        getValue: (inst) => calculatePinData0(inst),
                    },
                    {
                        // .pin.pinData1
                        /* For background calculation only */
                        name: "bslInvokePinData1",
                        displayName: "BSL Invoke Pin Details: Data1",
                        description: "BSL Invoke Pin Details: Data1",
                        longDescription: ``,
                        hidden: true,
                        readOnly: true,
                        displayFormat: "hex",
                        default: 0,
                        getValue: (inst) => calculatePinData1(inst),
                    },
                ]
            },
        ].concat(extendedConfigROMBSL).concat(
            {
                // .configID
                /* BSLID */
                name: "bslConfigID",
                displayName: "BSL Configuration ID",
                description: "Predetermined Config Signature Magic ID",
                longDescription: ``,
                hidden: false,
                readOnly: true,
                displayFormat: "hex",
                default: 0x05000000,
                range: [0, 0xFFFFFFFF]
            },
            {
                // .appRev
                /* BSLAPPVER */
                name: "bslAppVersion",
                displayName: "BSL App Version",
                description: "BSL App Version",
                longDescription: `
    The BSL supports returning an application version number through the BSL serial
            interface.This allows the BSL host to interrogate the firmware version without
    being able to read the firmware.The version field is 32 bits in length.

    To link the application version command to a version number programmed in the
    main flash, program the address of the version number in this field.

    The version data is only returned if the specified address corresponds to a
    valid flash memory address.
    `,
                hidden: false,
                displayFormat: "hex",
                default: 0xFFFFFFFF,
                range: [0, 0xFFFFFFFF]
            },
            {
                // .memoryRead
                /* BSLCFG.READOUTEN */
                name: "bslEnableReadOut",
                displayName: "BSL Read Out Enable",
                description: "Enables memory read-out via BSL",
                longDescription: `
    The BSL can be configured to allow read - out of memory locations through the BSL
    serial interface.The default BSL configuration on new devices does not enable
            read - out.
    `,
                hidden: false,
                default: true,
            },
            {
                // .securityAlert
                /* BSLSECCFG */
                name: "bslSecurityConfig",
                displayName: "BSL Security Alert Configuration",
                description: "BSL Security Alert Configuration",
                longDescription: `
    A brute force attack security alert occurs if three consecutive invalid
    passwords are provided to the BSL in an attempt to gain access to the BSL.The
    BSL can be configured to respond to the security alert in one of the following
            ways:

    * Issue a factory reset where MAIN and NONMAIN flash regions are erased.Only
    memory regions which are not protected by static write protection settings will
    be erased / reset.

    * Disable the bootloader(leaves MAIN intact but reconfigures NONMAIN to
    block BSL access).This option is only available if NONMAIN is not statically
    write protected.

    * Ignore(do not modify the configuration and allow password attempts to
            continue).
            `,
                hidden: false,
                default: "triggerFactoryReset",
                options: [
                    { name: "triggerFactoryReset", displayName: "Trigger a factory reset" },
                    { name: "disableBSL", displayName: "Disable BSL" },
                    { name: "ignoreAlert", displayName: "Ignore security alert" },
                ],
            },
            {
                // .userCfgCRC
                /* BSLCRC */
                name: "bslCRC",
                displayName: "Expected BSL Configuration CRC",
                description: "The expected CRC value to test against during boot",
                longDescription: ``,
                hidden: false,
                displayFormat: "hex",
                default: 0,
                getValue: (inst) => {
                    return calculateBSLCRC_32Bit(inst);
                },
            },
        ),
    },
]));

function calculatePinData1(inst) {
    let pinData1;

    if (!inst.bslDefaultInvokePin) {
        let pinID = Common.getGPIONumber(inst.bslInvokePin);
        let port = Common.getGPIOPort(inst.bslInvokePin);

        // Map GPIO ports to their correct index values
        let gpioBaseIndex;
        switch (port) {
            case "GPIOA":
                gpioBaseIndex = 0;
                break;
            case "GPIOB":
                gpioBaseIndex = 1;
                break;
            case "GPIOC":
                gpioBaseIndex = 2;
                break;
            default:
                gpioBaseIndex = 0;
        }

        pinData1 = pinID | (gpioBaseIndex << 5);
    }
    else {
        /* Default pin is PA18 for both M0L and M0G device families */
        pinData1 = 0x12;
    }

    return (pinData1);
}

function calculatePinData0(inst) {
    let pinData0;


    if (!inst.bslDefaultInvokePin) {
        // Find the pin data from iomuxPincmOptions
        let pinInfo = iomuxPincmOptions.find(x => x.name === inst.bslInvokePin);
        if (pinInfo) {
            // Use the num field from pinInfo instead of accessing attributes
            let pinCM = pinInfo.num;
            // If pinCM contains multiple values (e.g. "A,B"), take the first one
            if (pinCM.includes(',')) {
                pinCM = pinCM.split(',')[0];
            }
            // Convert to number and combine with level
            pinCM = parseInt(pinCM, 10);
            pinData0 = pinCM | (inst.bslInvokePinLevel << 7);
        } else {
            // Default fallback if pin not found
            pinData0 = 0x00;
        }
    } else {
        /* Default pincm depends on the device family */
        let pinCM = 40;  // Default value
        if (Common.isDeviceFamily_PARENT_MSPM0L122X_L222X()) {
            pinCM = 50;
        }
        /* Default pin level is high */
        pinData0 = pinCM | (1 << 7);
    }
    return pinData0;
}

function createUARTTXPinOptions(inst, ui) {
    let pinOptions = [];
    for (let i in uartTXPinData) {

        let filteredUARTPin = uartTXPinData[i].muxOptions.filter(a => a.peripheralPin.name == inst.uartPeripheral + ".TX")
        if (filteredUARTPin.length > 0) {
            pinOptions.push(
                { name: uartTXPinData[i].designSignalName }
            )
        }
    }

    return pinOptions;
}

function createUARTRXPinOptions(inst, ui) {
    let pinOptions = [];

    for (let i in uartRXPinData) {
        let filteredUARTPin = uartRXPinData[i].muxOptions.filter(a => a.peripheralPin.name == inst.uartPeripheral + ".RX")
        if (filteredUARTPin.length > 0) {
            pinOptions.push(
                { name: uartRXPinData[i].designSignalName }
            )
        }
    }

    return pinOptions;
}

function createI2CSCLPinOptions(inst, ui) {
    let pinOptions = [];

    for (let i in i2cSCLPinData) {
        let filteredPin = i2cSCLPinData[i].muxOptions.filter(a => a.peripheralPin.name == inst.i2cPeripheral + ".SCL")
        if (filteredPin.length > 0) {
            pinOptions.push(
                { name: i2cSCLPinData[i].designSignalName }
            )
        }
    }

    return pinOptions;
}

function createI2CSDAPinOptions(inst, ui) {
    let pinOptions = [];
    for (let i in i2cSDAPinData) {
        let filteredPin = i2cSDAPinData[i].muxOptions.filter(a => a.peripheralPin.name == inst.i2cPeripheral + ".SDA")
        if (filteredPin.length > 0) {
            pinOptions.push(
                { name: i2cSDAPinData[i].designSignalName }
            )
        }
    }
    return pinOptions;
}

function createMCANTXPinOptions(inst, ui) {
    let pinOptions = [];
    for (let i in mcanTXPinData) {
        let filteredMCANPin = mcanTXPinData[i].muxOptions.filter(a => a.peripheralPin.name == inst.mcanPeripheral + ".CANTX")
        if (filteredMCANPin.length > 0) {
            pinOptions.push(
                { name: mcanTXPinData[i].designSignalName }
            )
        }
    }
    return pinOptions;
}

function createMCANRXPinOptions(inst, ui) {
    let pinOptions = [];
    for (let i in mcanRXPinData) {
        let filteredMCANPin = mcanRXPinData[i].muxOptions.filter(a => a.peripheralPin.name == inst.mcanPeripheral + ".CANRX")
        if (filteredMCANPin.length > 0) {
            pinOptions.push(
                { name: mcanRXPinData[i].designSignalName }
            )
        }
    }
    return pinOptions;
}

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base module configuration
 */
let devSpecific = {

    /* This is where you should be putting all of your configurable information.
    If you want a configurable or variable that is not visible, you can make it
    hidden
     */

    config: nonMainConfig,

    pinmuxRequirements: pinmuxRequirements,

    filterHardware: filterHardware,

    validate: validateNONMAIN,

    _getPinResources: _getPinResources,
};

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic module module to
 *  allow us to augment/override as needed for the MSPM0.
 */
function extend(base) {
    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base.moduleStatic, devSpecific);

    /* concatenate device-specific configs */
    // moduleStatic specific to NONMAIN as it's statically defined
    result.config = base.moduleStatic.config.concat(devSpecific.config);
    base.moduleStatic = result;

    return (base);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base module module */
    extend: extend
};
