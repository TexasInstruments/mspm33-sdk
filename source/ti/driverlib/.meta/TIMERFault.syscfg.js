/*
 * Copyright (c) 2026, Texas Instruments Incorporated - http://www.ti.com
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
let Common = system.getScript("/ti/driverlib/Common.js");
let EVENT = system.getScript("/ti/driverlib/EVENT.syscfg.js");

let config = [
    {
        name: "$name",
        defaultPrefix: "FAULT_",
        hidden: false
    },
    {
        name: "enabledFaults",
        displayName: "Configure Fault Pins",
        longDescription: `
Fault Pin Configuration can be shared across Timer peripherals. The selected
fault pin signals will be shared with any other instances that also enable the
same fault pin as source, unless configured otherwise as a new fault instance.`,
        default: [],
        minSelections: 0,
        options: [
            {name: "0", displayName: "Fault Pin 0"},
            {name: "1", displayName: "Fault Pin 1"},
            {name: "2", displayName: "Fault Pin 2"},
        ]
    }
];

/* Add Pinmux Peripheral Configuration group */
config = config.concat(Common.getGPIOGroupConfig());

const memoizedGPIOFilter = _.memoize(curryGPIOFilter, (validPinSet) => JSON.stringify(validPinSet));

function curryGPIOFilter(validPinSet) {
	return (devicePin, peripheralPin) => GPIOFilter(devicePin, validPinSet);
}

function GPIOFilter(devicePin, validPinSet) {
    return validPinSet.includes(devicePin.name);
}

/* Filter valid pins for Fault Pin configuration */
function validFaultPinSet(inst, faultNumber) {
    let validPinSet = [];

    if (Common.isDeviceM33x321x()) {
        if (Common.hasTimerA()) { /* Necessary to allow migration to an M0L11XX_L13XX device */
            for (let pinIdx in system.deviceData.gpio.pinInfo) {
                // Get the fault pin mappings array
                const faultPinMappings = system.deviceData.interfaces.GPTIMER.interfacePins["FAULT" + faultNumber].pinMappings;

                // Iterate through the actual array elements, not the length property
                for (let pinMappingIdx = 0; pinMappingIdx < faultPinMappings.length; pinMappingIdx++) {
                    let eligible = true;

                    // Get the fault array for this mapping
                    let theFaultArray = faultPinMappings[pinMappingIdx].pinMappings;
                    let faultIDArray = _.map(theFaultArray, 'id');

                    // Check if the current pin's ID is in the fault ID array
                    if (!faultIDArray.includes(system.deviceData.gpio.pinInfo[pinIdx].devicePin.id)) {
                        eligible = false;
                    }

                    // If the pin is eligible, add it to the valid pin set and break the inner loop
                    if (eligible) {
                        validPinSet.push(pinIdx);
                    }
                }
            }
        }
    } else {
        if(Common.hasTimerA()){ /* Necessary to allow migration to an M0L11XX_L13XX device */
            for(let pinIdx in system.deviceData.gpio.pinInfo){
                let eligible = true;

                let theFaultArray = system.deviceData.interfaces.GPTIMER.interfacePins["FAULT"+faultNumber].pinMappings[0].pinMappings;
                let faultIDArray = _.map(theFaultArray, 'id');

                if (!faultIDArray.includes(system.deviceData.gpio.pinInfo[pinIdx].devicePin.id)){
                    eligible = false;
                }

                if(eligible){
                    validPinSet.push(pinIdx);
                }
            }
        }
    }

    return validPinSet;
}

/*  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  param inst    - a fully configured module instance
 *
 *  returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let faultResources = [];
    let allResources = [
        {
            name: "faultPin0",
            displayName: "Fault Pin 0",
            // interfaceNames: ["FAULT0"],
            interfaceName: "GPIO",
            filter: memoizedGPIOFilter(validFaultPinSet(inst,0)),
        },
        {
            name: "faultPin1",
            displayName: "Fault Pin 1",
            // interfaceNames: ["FAULT1"],
            interfaceName: "GPIO",
            filter: memoizedGPIOFilter(validFaultPinSet(inst,1)),
        },
        {
            name: "faultPin2",
            displayName: "Fault Pin 2",
            // interfaceNames: ["FAULT2"],
            interfaceName: "GPIO",
            filter: memoizedGPIOFilter(validFaultPinSet(inst,2)),
        },
    ];
    for(let faultIdx = 0; faultIdx <= 2; faultIdx++){
        if((inst.enabledFaults).includes(faultIdx.toString())){
            faultResources.push(allResources[faultIdx]);
        }
    }

    return (faultResources);
}



function validate(inst, validation)
{
    /* Validate configured Fault Pins are enabled as needed */
    let unusedConfigured = [false,false,false];
    for(let parentModule of inst.$sharedBy){
        if(parentModule){
            if(parentModule.faultHandlerEn){
                /* check if fault pins enabled as sources */
                for(let faultIdx = 0; faultIdx <= 2; faultIdx++){
                    if(parentModule.faultSource.includes(faultIdx.toString()) && !(inst.enabledFaults.includes(faultIdx.toString()))){
                        validation.logError("Fault Pin "+faultIdx+" is not enabled and is being configured by "+parentModule.$name, inst, "enabledFaults");
                    }

                    else if(parentModule.faultSource.includes(faultIdx.toString()) && (inst.enabledFaults.includes(faultIdx.toString()))){
                        unusedConfigured[faultIdx] = true;
                    }
                }
            }
        }
    }
    for(let faultIdx = 0; faultIdx <= 2; faultIdx++){
        if(inst.enabledFaults.includes(faultIdx.toString()) && !unusedConfigured[faultIdx]){
            validation.logInfo("Fault Pin "+faultIdx+" is not being used by any Timer peripheral", inst, "enabledFaults");
        }
    }

    if(inst.$ownedBy){
        validation.logWarning("This is a test warning",inst);
    }
}

function validatePinmux(inst, validation)
{

}

function moduleInstances(inst){
    let modInstances = [];

    /*
     * Gets a Peripheral GPIO Configuration submodule
     */
    /* CONDITIONS CODE START */
    // Fault Pins 0-2
    let timerConfig = [false, false, false];

    for(let faultIdx = 0; faultIdx <= 2; faultIdx++){
        if((inst.enabledFaults).includes(faultIdx.toString())){
            timerConfig[faultIdx] = true;
        }
    }
    /* CONDITIONS CODE END */

    /* Fault Pin 0 */
    if(timerConfig[0]){
        let mod = {
            name: "faultPin0"+"Config",
            displayName: "Fault Pin 0",
            moduleName: '/ti/driverlib/gpio/GPIOPinGeneric',
            group: "GROUP_PINMUX_PERIPH",
            collapsed: true,
            requiredArgs: {
                passedPinName: "GPIO_"+inst.$name+"_IOMUX_"+"FAULT_PIN_0",
                passedResourceName: "faultPin0",
                passedPeripheralType: "Digital",
                direction: "INPUT",
                hideOutputInversion: false,
            },
        }
        modInstances.push(mod);
    }
    /* Fault Pin 1 */
    if(timerConfig[1]){
        let mod = {
            name: "faultPin1"+"Config",
            displayName: "Fault Pin 1",
            moduleName: '/ti/driverlib/gpio/GPIOPinGeneric',
            group: "GROUP_PINMUX_PERIPH",
            collapsed: true,
            requiredArgs: {
                passedPinName: "GPIO_"+inst.$name+"_IOMUX_"+"FAULT_PIN_1",
                passedResourceName: "faultPin1",
                passedPeripheralType: "Digital",
                direction: "INPUT",
                hideOutputInversion: false,
            },
        }
        modInstances.push(mod);
    }
    /* Fault Pin 2 */
    if(timerConfig[2]){
        let mod = {
            name: "faultPin2"+"Config",
            displayName: "Fault Pin 2",
            moduleName: '/ti/driverlib/gpio/GPIOPinGeneric',
            group: "GROUP_PINMUX_PERIPH",
            collapsed: true,
            requiredArgs: {
                passedPinName: "GPIO_"+inst.$name+"_IOMUX_"+"FAULT_PIN_2",
                passedResourceName: "faultPin2",
                passedPeripheralType: "Digital",
                direction: "INPUT",
                hideOutputInversion: false,
            },
        }
        modInstances.push(mod);
    }

    return (modInstances);
}

exports = {
    config: config,
    pinmuxRequirements: pinmuxRequirements,
    validate: validate,
    validatePinmux: validatePinmux,
    modules:        Common.autoForceModules(["SYSCTL","Board"]),
    moduleInstances: moduleInstances,
    templates: {
        boardc : "/ti/driverlib/timer/TIMERFault.Board.c.xdt",
        boardh : "/ti/driverlib/timer/TIMERFault.Board.h.xdt",
        Call: false,
        Reset: false,
        Power: false,
        GPIO: true,
        Function: false
    },
    /* Added for cleaner error message on duplicate pin name */
    displayName: "Timer Fault",
};
