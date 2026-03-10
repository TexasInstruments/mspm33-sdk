/*
 * Copyright (c) 2025 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== SHAMSPM33.syscfg.js ========
 */

"use strict";

/* get Common /ti/driverlib utility functions */
let Common = system.getScript("/ti/driverlib/Common.js");

/* Add these at the top with other constants */let dmaEvt1Options = [
    { name: "None", displayName: "None" },
    { name: "DL_SHAW_TRIGGER", displayName: "SHA DMA Trigger" },
];


/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst) {

}

/*
 *  ======== validatePinmux ========
 *  Validation run after solution
 *
 *  param inst       - module instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validatePinmux(inst, validation) {

}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  param inst       - module instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation) {

}


/*  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  param inst    - a fully configured module instance
 *
 *  returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst) {
    /* SHA has no pinmux requirements */
    return ([]);
}

function onChangeSetCustomProfile(inst, ui) {
    inst.profile = "CUSTOM";
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

const SHAModeOptions = [
    { name: "DL_SHAW_MODE_HASH", displayName: "HASH Mode" },
    { name: "DL_SHAW_MODE_HMAC", displayName: "HMAC Mode" },
];

const SHAAlgorithmOptions = [
    { name: "DL_SHAW_ALGORITHM_SHA_224", displayName: "SHA_224" },
    { name: "DL_SHAW_ALGORITHM_SHA_256", displayName: "SHA_256" },
];

let SHAConfig = [
    {
        name: "GROUP_PROFILE",
        displayName: "Quick Profiles",
        description: "",
        collapsed: false,
        config: [
            {
                name: "profile",
                displayName: "SHAW Profiles",
                description: 'Pre-defined profiles for typical SHAW configurations',
                longDescription: ``,
                hidden: true, //TODO: keep hidden until profiles are defined
                default: "TODO: PLACEHOLDER FOR NOW",
            },
        ],
    },

    {
        /* DL_SHAW_Config.mode */
        name: "mode",
        displayName: "SHA Mode",
        description: "The SHA mode",
        hidden: false,
        default: "DL_SHAW_MODE_HASH",
        options: SHAModeOptions,
        // onChange: onChangeMode,
    },

    {
        /* DL_SHAW_Config.mode */
        name: "algorithm",
        displayName: "SHA Algorithm",
        description: "The SHA Algorithm",
        hidden: false,
        default: "DL_SHAW_ALGORITHM_SHA_224",
        options: SHAAlgorithmOptions,
        // onChange: onChangeMode,
    },

]

SHAConfig = SHAConfig.concat(getInterruptGroupConfig());
SHAConfig = SHAConfig.concat(getDMAGroupConfig());

function getInterruptOptions() {
    let options = [
        { name: "DL_SHAW_enableOutputReadyInterrupt", displayName: "Output Ready Interrupt" },
        { name: "DL_SHAW_enableDMATriggerInterrupt", displayName: "DMA Trigger Interrupt" },
    ];

    return options;
};

function onChangeInterrupts(inst, ui) {
    updateGUIInterrupts(inst, ui);
    // onChangeCustomProfile(inst,ui);
}

function updateGUIInterrupts(inst, ui) {
    ui.interruptPriority.hidden = (inst.enabledInterrupts.length === 0);
}

function getInterruptGroupConfig() {
    return [
        {
            name: "GROUP_INTERRUPTS",
            displayName: "Interrupt Configuration",
            description: "",
            longDescription: "This section allows enabling/disabling SHA interrupts",
            collapsed: true,
            config: [
                {
                    name: "enabledInterrupts",
                    displayName: "Enable Interrupts",
                    description: "Enable Interrupts",
                    hidden: false,
                    default: [
                        "DL_SHAW_enableOutputReadyInterrupt",
                        "DL_SHAW_enableDMATriggerInterrupt",
                    ],
                    options: getInterruptOptions,
                    onChange: onChangeInterrupts,
                },
                {
                    name: "interruptPriority",
                    displayName: "Interrupt Priority",
                    description: "Specifies interrupt priority",
                    longDescription: '',
                    hidden: true,
                    default: "DEFAULT",
                    options: Common.InterruptPriorityOptions,
                },
            ]
        },
    ];
} function getDMAGroupConfig() {
    return [
        {
            name: "GROUP_DMA",
            displayName: "DMA Configuration",
            longDescription: "This section allows configuring DMA Event triggers for SHAW",
            collapsed: true,
            config: [
                {
                    name: "enabledDMAEvent1Triggers",
                    displayName: "Configure DMA Event Triggers",
                    description: 'Configure the SHAW DMA trigger event',
                    hidden: false,
                    default: "None",
                    options: dmaEvt1Options,
                    onChange: (inst, ui) => {
                        ui.enableDMAEvent1.hidden = (inst.enabledDMAEvent1Triggers == "None");
                        onChangeSetCustomProfile(inst, ui);
                    },
                },
                {
                    name: "enableDMAEvent1",
                    displayName: "Enable DMA Event Trigger",
                    longDescription: "If left unchecked, the module-specific enable DMA API should be called in the application code.",
                    hidden: true,
                    default: true,
                    // onChange: onChangeSetCustomProfile,
                }
            ]
        }
    ];
}

function moduleInstances(inst) {
    let modInstances = []

    if (!["None"].includes(inst.enabledDMAEvent1Triggers)) {
        let mod = {
            name: "DMA_CHANNEL_EVENT1",
            displayName: "DMA Channel Event 1",
            moduleName: '/ti/driverlib/DMAChannel',
            group: "GROUP_DMA",
            args: {

            },
            requiredArgs: {
                hideTriggerSelect: true,
            },
        }
        modInstances.push(mod);
    }

    return modInstances;
}

function setRequiredModules(inst) {
    let theModules = ["Board", "SYSCTL"]

    let kwargs = theModules;
    let modArray = [];

    for (let args = kwargs.length - 1; args >= 0; args--) {
        let modPath = kwargs[args];
        if (modPath.indexOf('/') == -1) {
            modPath = "/ti/driverlib/" + modPath;
        }
        modArray.push({
            name: modPath.substring(modPath.lastIndexOf('/') + 1),
            moduleName: modPath,
            hidden: true
        });
    }

    return modArray
};


let devSpecific = {

    config: SHAConfig,

    /* override generic requirements with  device-specific reqs (if any) */
    validate: validate,

    validatePinmux: validatePinmux,

    pinmuxRequirements: pinmuxRequirements,

    filterHardware: filterHardware,

    moduleInstances: moduleInstances,

    modules: setRequiredModules,

    _getPinResources: _getPinResources
};

let config = [
    /* Show selected peripheral below instance name */
    {
        name: "selectedInstance",
        displayName: "Selected Peripheral",
        default: "",
        longDescription: `Refer to the PinMux section for peripheral selection. Chosen instance name is not indicative of active peripheral.`,
        getValue: (inst) => {
            let solution = "";
            try {
                solution = inst.peripheral.$solution.peripheralName;
            } catch (e) {
                // do nothing
            }
            return solution;
        }
    },
    // {
    //     name: "GROUP_PROFILE",
    //     displayName: "Quick Profiles",
    //     description: "",
    //     collapsed: false,
    //     config: [
    //         {
    //             name: "profile",
    //             displayName: "SHA Profiles",
    //             description: "Pre-defined profiles for typical SHA configurations",
    //             longDescription: ``,
    //             hidden: false,
    //             default: "CUSTOM",
    //             options: [
    //                 {name: "CUSTOM", displayName: "Custom"},
    //             ],
    //             onChange: onChangeSHAProfile,
    //         },
    //     ],
    // },
]

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic module module to
 *  allow us to augment/override as needed for the MSP432.
 */
function extend(base) {
    /* Special static extend */
    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base.moduleStatic, devSpecific);

    /* concatenate device-specific configs */
    /* moduleStatic specific to SHAW as it's statically defined */
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
