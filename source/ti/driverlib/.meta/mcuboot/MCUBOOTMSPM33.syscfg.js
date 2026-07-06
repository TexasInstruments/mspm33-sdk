/*
 * Copyright (c) 2024, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== MCUBOOTMSPM33.syscfg.js ========
 */

"use strict";

/* get Common /ti/driverlib utility functions */
let Common = system.getScript("/ti/driverlib/Common.js");

/* Define MCUboot default settings for MSPM33 */
let mcubootSettings = {
    tzEnabled: { readOnly: false, hidden: false },
    image1: {
        primaryBase: 0x8000,
        primarySize: 0x30000,
        secondaryBase: 0x38000,
        secondarySize: 0x30000
    },
    image2: {
        primaryBase: 0x68000,
        primarySize: 0xA000,
        secondaryBase: 0x72000,
        secondarySize: 0xA000
    },
    antiRollbackProtection: { readOnly: false, hidden: false },
    bootloader: { base: 0x0, size: 0x8000 },
    alignment: { flashBoundary: 0x80000, sectorSize: 0x1000 }
};

/* Device group identifier */
let deviceGroup = "DeviceGroup_MSPM33";

/* External flash settings if needed */
let externalFlashSectorSize = 0x1000;
let externalFlashSize = 0x400000;
let externalFlashBase = 0x0;

let logError = Common.logError;
let logWarning = Common.logWarning;

/*  ======== validatePinmux ========
 *  Validation run after solution
 */
function validatePinmux(inst, validation) {
    // No pinmux requirements for MCUboot
}

/*  ======== validate ========
 *  Validate this inst's configuration
 */
function validate(inst, validation) {

    // if instance of trustzone is true, flash base must be appropriate
    if (inst.tzEnabled && inst.bootloaderBaseAddress != 0x0) {
        logError(validation, inst, "bootloaderBaseAddress",
            "Value must be 0x0 if Trust Zones are enabled on MSPM33");
    }

    // if size of primary image is not the same as secondary image throw error
    if (inst.primarySize1 !== inst.secondarySize1 && inst.mode == 'MCUBOOT_OVERWRITE_MODE') {
        logError(validation, inst, "primarySize1",
            "Primary image must be the same size as secondary image");
        logError(validation, inst, "secondarySize1",
            "Secondary image must be the same size as primary image");
    }

    // if primary image starts less than bootloader base + bootloader size, throw error
    if (inst.bootloaderBaseAddress + inst.bootloaderSize > inst.primaryBase1) {
        logError(validation, inst, "bootloaderBaseAddress",
            "The base of the primary image must be greater than or equal to the base of the bootloader plus the size of the bootloader.");
        logError(validation, inst, "bootloaderSize",
            "The base of the primary image must be greater than or equal to the base of the bootloader plus the size of the bootloader.");
        logError(validation, inst, "primaryBase1",
            "The base of the primary image must be greater than or equal to the base of the bootloader plus the size of the bootloader.");
    }

    // check if the start address for bootloader and images are sector aligned
    if (inst.primaryBase1 % mcubootSettings.alignment.sectorSize !== 0) {
        logError(validation, inst, "primaryBase1",
            "The base of the primary image must be sector aligned.");
    }

    if (inst.secondaryBase1 % mcubootSettings.alignment.sectorSize !== 0) {
        logError(validation, inst, "secondaryBase1",
            "The base of the secondary image must be sector aligned.");
    }

    if (inst.bootloaderBaseAddress % mcubootSettings.alignment.sectorSize !== 0) {
        logError(validation, inst, "bootloaderBaseAddress",
            "The base of the bootloader must be sector aligned.");
    }

    if (inst.bootloaderSize % mcubootSettings.alignment.sectorSize !== 0) {
        logError(validation, inst, "bootloaderSize",
            "The size of the bootloader must be a multiple of the sector size.");
    }

    // if image layout overlaps each other, throw error
    if (inst.secondaryBase1 >= inst.primaryBase1 && inst.primaryBase1 + inst.primarySize1 > inst.secondaryBase1) {
        logError(validation, inst, "primaryBase1",
            "Primary image and secondary image overlap each other.");
        logError(validation, inst, "primarySize1",
            "Primary image and secondary image overlap each other.");
        logError(validation, inst, "secondaryBase1",
            "Primary image and secondary image overlap each other.");
        logError(validation, inst, "secondarySize1",
            "Primary image and secondary image overlap each other.");
    }
    else if (inst.primaryBase1 >= inst.secondaryBase1 && inst.secondaryBase1 + inst.secondarySize1 > inst.primaryBase1) {
        logError(validation, inst, "primaryBase1",
            "Primary image and secondary image overlap each other.");
        logError(validation, inst, "primarySize1",
            "Primary image and secondary image overlap each other.");
        logError(validation, inst, "secondaryBase1",
            "Primary image and secondary image overlap each other.");
        logError(validation, inst, "secondarySize1",
            "Primary image and secondary image overlap each other.");
    }

    // check if non secure image sizes and bases are within bounds
    if (inst.tzEnabled === true) {
        if (inst.primarySize2 !== inst.secondarySize2) {
            logError(validation, inst, "primarySize2",
                "Primary image must be the same size as secondary image");
            logError(validation, inst, "secondarySize2",
                "Secondary image must be the same size as primary image");
        }

        // Check if secondary slots don't overlap
        if (inst.secondaryBase1 + inst.secondarySize1 > inst.secondaryBase2 &&
            inst.secondaryBase2 + inst.secondarySize2 > inst.secondaryBase1) {
            logError(validation, inst, "secondaryBase1",
                "Secondary images overlap each other.");
            logError(validation, inst, "secondarySize1",
                "Secondary images overlap each other.");
            logError(validation, inst, "secondaryBase2",
                "Secondary images overlap each other.");
            logError(validation, inst, "secondarySize2",
                "Secondary images overlap each other.");
        }

        // Check if non-secure slots are sector aligned
        if (inst.primaryBase2 % mcubootSettings.alignment.sectorSize !== 0) {
            logError(validation, inst, "primaryBase2",
                "The base of the non-secure primary image must be sector aligned.");
        }

        if (inst.secondaryBase2 % mcubootSettings.alignment.sectorSize !== 0) {
            logError(validation, inst, "secondaryBase2",
                "The base of the non-secure secondary image must be sector aligned.");
        }

        // Check if non-secure slots are the right size
        if (inst.primarySize2 % mcubootSettings.alignment.sectorSize !== 0) {
            logError(validation, inst, "primarySize2",
                "The size of the non-secure primary image must be a multiple of the sector size.");
        }

        if (inst.secondarySize2 % mcubootSettings.alignment.sectorSize !== 0) {
            logError(validation, inst, "secondarySize2",
                "The size of the non-secure secondary image must be a multiple of the sector size.");
        }
    }
}

/* Event handlers for UI changes */
function onChangeMode(inst, ui) {

}

function changeTzEnable(inst, ui) {
    // change mode to overwrite when TrustZone is enabled
    inst.mode = 'MCUBOOT_OVERWRITE_MODE';
    ui.mode.readOnly = inst.tzEnabled;

    // set hidden properties for second image slots
    ui.primaryBase2.hidden = !inst.tzEnabled;
    ui.primarySize2.hidden = !inst.tzEnabled;
    ui.secondaryBase2.hidden = !inst.tzEnabled;
    ui.secondarySize2.hidden = !inst.tzEnabled;

    // set readonly properties when TZ is enabled
    ui.primaryBase1.readOnly = inst.tzEnabled;
    ui.primarySize1.readOnly = inst.tzEnabled;
    ui.secondaryBase1.readOnly = inst.tzEnabled;
    ui.secondarySize1.readOnly = inst.tzEnabled;
    ui.bootloaderBaseAddress.readOnly = inst.tzEnabled;
    ui.bootloaderSize.readOnly = inst.tzEnabled;

    // if TrustZone is enabled, set predefined memory layout
    if (inst.tzEnabled) {
        // set bootloader size and base for TrustZone configuration
        inst.bootloaderBaseAddress = 0x0;
        inst.bootloaderSize = 0x6000;

        // set secure image slots
        inst.primaryBase1 = 0x6000;
        inst.primarySize1 = 0x24000;
        inst.secondaryBase1 = 0x38000;
        inst.secondarySize1 = 0x24000;

        // set non-secure image slots
        inst.primaryBase2 = 0x2A000;
        inst.primarySize2 = 0x0E000;
        inst.secondaryBase2 = 0x5C000;
        inst.secondarySize2 = 0x0E000;
    } else {
        // else revert to default single-image configuration
        inst.bootloaderBaseAddress = mcubootSettings.bootloader.base;
        inst.bootloaderSize = mcubootSettings.bootloader.size;
        inst.primaryBase1 = mcubootSettings.image1.primaryBase;
        inst.primarySize1 = mcubootSettings.image1.primarySize;
        inst.secondaryBase1 = mcubootSettings.image1.secondaryBase;
        inst.secondarySize1 = mcubootSettings.image1.secondarySize;
        inst.primaryBase2 = mcubootSettings.image2.primaryBase;
        inst.primarySize2 = mcubootSettings.image2.primarySize;
        inst.secondaryBase2 = mcubootSettings.image2.secondaryBase;
        inst.secondarySize2 = mcubootSettings.image2.secondarySize;
    }
}

/* Configure module options */
const MCUBootUpgradeModeOptions = [
    { name: "MCUBOOT_OVERWRITE_MODE", displayName: "Overwrite" },
    { name: "MCUBOOT_XIP_MODE", displayName: "XIP" },
];

/* MCUBoot configuration UI elements */
let MCUBOOTConfig = [
    {
        name: "mode",
        displayName: "Upgrade Mode",
        description: "Select how MCUboot handles updates",
        longDescription: "TI supports two upgrade methods - Overwrite and XIP",
        hidden: false,
        default: "MCUBOOT_OVERWRITE_MODE",
        options: MCUBootUpgradeModeOptions,
    },
    {
        name: "tzEnabled",
        displayName: "Enable TrustZone",
        description: "Enable MCUboot to store 2 images - secure and non-secure image.",
        default: false,
        readOnly: false,
        hidden: false,
        onChange: changeTzEnable
    },
    {
        name: "enableECDSA",
        displayName: "Authenticate using ECDSA",
        description: "Authenticate using ECDSA",
        default: false,
        hidden: false
    },
    {
        name: "enableMLDSA",
        displayName: "Authenticate using MLDSA",
        description: "Authenticate using MLDSA",
        default: false,
        hidden: false
    },
    {
        name: "enablebothECDSAMLDSA",
        displayName: "Authenticate using ECDSA and then MLDSA",
        description: "Authenticate using ECDSA and then MLDSA",
        default: false,
        hidden: false
    },
    {
        name: "enableHWSHA256",
        displayName: "Use HW SHA256",
        description: "Use HW SHA256",
        default: false,
        hidden: false
    },
    {
        name: "antiRollbackProtection",
        displayName: "Anti Rollback Protection",
        description: "Enable hardware-based downgrade prevention using security counters",
        default: false,
        readOnly: false,
        hidden: false
    },
    {
        displayName: "Bootloader",
        description: "Bootloader Configurations",
        collapsed: true,
        config: [
            {
                name: "bootloaderBaseAddress",
                displayName: "Base",
                description: "Base Address of MCUboot",
                displayFormat: { radix: "hex", bitSize: 32 },
                default: mcubootSettings.bootloader.base,
                readOnly: false,
                hidden: false
            },
            {
                name: "bootloaderSize",
                displayName: "Size",
                description: "Size of MCUboot",
                displayFormat: { radix: "hex", bitSize: 32 },
                default: mcubootSettings.bootloader.size,
                readOnly: false,
                hidden: false
            }
        ]
    },
    {
        name: "image1",
        displayName: "Image 1",
        description: "Image 1 (Secure image if TrustZone is enabled)",
        collapsed: true,
        config: [
            {
                displayName: "Primary Image",
                description: "Image stored in primary slot",
                collapsed: false,
                config: [
                    {
                        name: "primaryBase1",
                        displayName: "Base Address",
                        description: "Base Address of Primary Image",
                        displayFormat: { radix: "hex", bitSize: 32 },
                        default: mcubootSettings.image1.primaryBase,
                        readOnly: false,
                        hidden: false
                    },
                    {
                        name: "primarySize1",
                        displayName: "Image Size",
                        description: "Size of Primary Image",
                        displayFormat: { radix: "hex", bitSize: 32 },
                        default: mcubootSettings.image1.primarySize,
                        readOnly: false,
                        hidden: false
                    }
                ]
            },
            {
                displayName: "Secondary Image",
                description: "Image stored in secondary slot",
                collapsed: false,
                config: [
                    {
                        name: "secondaryBase1",
                        displayName: "Base Address",
                        description: "Base Address of Secondary Image",
                        displayFormat: { radix: "hex", bitSize: 32 },
                        default: mcubootSettings.image1.secondaryBase,
                        readOnly: false,
                        hidden: false
                    },
                    {
                        name: "secondarySize1",
                        displayName: "Image Size",
                        description: "Size of Secondary Image",
                        displayFormat: { radix: "hex", bitSize: 32 },
                        default: mcubootSettings.image1.secondarySize,
                        readOnly: false,
                        hidden: false
                    }
                ]
            }
        ]
    },
    {
        name: "image2",
        displayName: "Image 2",
        description: "Non-secure image",
        collapsed: true,
        config: [
            {
                displayName: "Primary Image",
                description: "Image stored in primary slot",
                collapsed: false,
                config: [
                    {
                        name: "primaryBase2",
                        displayName: "Base",
                        description: "Base of Non-secure Primary Image",
                        displayFormat: { radix: "hex", bitSize: 32 },
                        default: mcubootSettings.image2.primaryBase,
                        readOnly: false,
                        hidden: true
                    },
                    {
                        name: "primarySize2",
                        displayName: "Size",
                        description: "Size of Non-secure Primary Image",
                        displayFormat: { radix: "hex", bitSize: 32 },
                        default: mcubootSettings.image2.primarySize,
                        readOnly: false,
                        hidden: true
                    }
                ]
            },
            {
                displayName: "Secondary Image",
                description: "Image stored in secondary slot",
                collapsed: false,
                config: [
                    {
                        name: "secondaryBase2",
                        displayName: "Base",
                        description: "Base of Non-secure Secondary Image",
                        displayFormat: { radix: "hex", bitSize: 32 },
                        default: mcubootSettings.image2.secondaryBase,
                        readOnly: false,
                        hidden: true
                    },
                    {
                        name: "secondarySize2",
                        displayName: "Size",
                        description: "Size of Non-secure Secondary Image",
                        displayFormat: { radix: "hex", bitSize: 32 },
                        default: mcubootSettings.image2.secondarySize,
                        readOnly: false,
                        hidden: true
                    }
                ]
            }
        ]
    }
];

/* Generate linker definitions */
function getLinkerDefs(inst) {
    let flashSize = inst.bootloaderSize;
    return [
        { name: "MCUBOOT_FLASH_BASE", value: inst.bootloaderBaseAddress },
        { name: "MCUBOOT_FLASH_SIZE", value: flashSize }
    ];
}

/* Required modules */
function setRequiredModules(inst) {
    let theModules = ["Board", "SYSCTL"];
    let modArray = [];

    for (let args = theModules.length - 1; args >= 0; args--) {
        let modPath = "/ti/driverlib/" + theModules[args];
        modArray.push({
            name: theModules[args],
            moduleName: modPath,
            hidden: true
        });
    }

    return modArray;
}

/* Device-specific configuration */
let devSpecific = {
    config: MCUBOOTConfig,
    validate: validate,
    validatePinmux: validatePinmux,
    modules: setRequiredModules,
    getLinkerDefs: getLinkerDefs
};

/* Extend base exports */
function extend(base) {
    /* Special static extend */
    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base.moduleStatic, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.moduleStatic.config.concat(devSpecific.config);
    base.moduleStatic = result;

    return (base);
}

/* Export module */
exports = {
    extend: extend,
    deviceGroup: deviceGroup,
    mcubootSettings: mcubootSettings,
    externalFlashSectorSize: externalFlashSectorSize,
    externalFlashSize: externalFlashSize,
    externalFlashBase: externalFlashBase
};
