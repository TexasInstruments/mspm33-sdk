/*
 * Copyright (c) 2025, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== SHA.syscfg.js ========
 */
"use strict";

/* get Common /ti/driverlib utility functions */
let Common = system.getScript("/ti/driverlib/Common.js");

/* generic configuration parameters for SHA instances */
let SHAConfig = []; /* nothing (yet) beyond generic driver configs */

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


/*
 *  ======== base ========
 *  Define the base SHA properties and methods
 */
let base = {
    displayName: "SHA",
    description: "SHA Module for MSPM33",
    longDescription: `
The **SHA module** allows the user to configure SHA modes and hash sizes, enable DMA trigger

Check the datasheet of your selected device for more detailed information.
    `,
    defaultInstanceName: "SHA",
    uiAdd: "staticOnly",
    moduleStatic: {
        config: SHAConfig,
        modules: Common.autoForceModules(["Board", "SYSCTL", "EVENT"]),
        /* override device-specific templates */

    },
    templates: {
        boardc: "/ti/driverlib/sha/SHA.Board.c.xdt",
        boardh: "/ti/driverlib/sha/SHA.Board.h.xdt",
        Call: true,
        Reset: true,
        Power: true,
        GPIO: true,
        Function: true
    },

    validate: validate,

    _getPinResources: _getPinResources // Ignore
};

let deviceSHA = system.getScript("/ti/driverlib/sha/SHAMSPM33");

exports = deviceSHA.extend(base);
