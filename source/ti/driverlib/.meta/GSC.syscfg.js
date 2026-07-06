"use strict";

let Common = system.getScript("/ti/driverlib/Common.js");

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  param inst       - module instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation) {
    Common.validateNames(inst, validation);
}

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst) {
    return;
}

// Define base module
let base = {
    displayName: "GSC",
    description: "Global Security Controller Configuration",
    longDescription: `The GSC module provides configuration for:
    - Memory protection
    - Secure/non-secure access control
    - Privilege level control
    - Flash protection`,
    defaultInstanceName: "CONFIG_GSC_",

    // Instance limitation
    useArray: true,
    defaultInstanceCount: 1,
    minInstanceCount: 1,
    maxInstanceCount: 1,

    modules: Common.autoForceModules(["Board"]),

    templates: {
        boardc: "/ti/driverlib/gsc/GSC.Board.c.xdt",
        boardh: "/ti/driverlib/gsc/GSC.Board.h.xdt",
        Call: true,      // Enable init function call generation
        Reset: true,     // Enable reset function generation  
        Power: false,    // Disable power management if not needed
        GPIO: false,     // Disable GPIO if not needed
        Function: true   // Enable function definition generation
    },

    validate: validate,
    _getPinResources: _getPinResources
};

// Get device-specific GSC implementation
let deviceGSC = system.getScript("/ti/driverlib/gsc/GSCMSPM33");

// The configuration will be provided by GSCMSPM33.syscfg.js through the extend() function
exports = deviceGSC.extend(base);