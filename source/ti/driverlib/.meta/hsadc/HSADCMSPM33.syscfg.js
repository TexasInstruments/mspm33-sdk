/*
 * Copyright (c) 2023-2024 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== HSADCMSPM33.syscfg.js ========
 */

"use strict";

/* get Common /ti/driverlib utility functions */
let Common = system.getScript("/ti/driverlib/Common.js");
let EVENT = system.getScript("/ti/driverlib/EVENT.syscfg.js");
let device_driverlib_peripheral = system.getScript("/ti/driverlib/hsadc/mspm33_hsadc.js");
let device_adc_char = system.getScript("/ti/driverlib/hsadc/hsadc_char.js");
var ESTIMATED_CP = 10.00001;
var ADC_SysClk_MHz = 160;
var ADC_SampleTime_SysClk_ns = 1000/ADC_SysClk_MHz;
let InternalConnections = system.getScript("/ti/driverlib/hsadc/HSADC_internalConnections.js");
/* SOC range varies per device family */
const hsadcSOCRange = (Common.isDeviceM33x321x()) ? 16:11;
const NO_DEVICE_PIN_FOUND = "No device pin found";

const longDescriptionSocTriggers = "SOC Trigger Configuration";
const longDescriptionSocSamplingTimeCalculator = "Sample Time Calculator Configuration";
const SOCTrigger_configs = [];
const defaultADCPinInfos = [];
const defaultPinCPValue = 0.5;
var DO_NOT_COPY_configNames = [
    "adcBase",
    "copyUse",
    "copyFrom",
    "copyPerform",
    "recalculateSampleTimes"
];

function calculateDevicePinName(inst, sociX) {
    let pinInfo = "";
    if (inst.peripheral && inst.peripheral.$solution) {
        let baseName = inst.peripheral.$solution.peripheralName;
        let hsadcName = "HS" + baseName;  // HSADC0 or HSADC1
        let sociChannel = inst["soc" + sociX.toString() + "Channel"].replace("ADC_CH_ADCIN_", "");
        let channelNum = parseInt(sociChannel);
        let peripheral = system.deviceData.peripherals[baseName];
        if (peripheral) {
            // First check if this is an internal channel for this specific HSADC instance
            let internalSignal = InternalConnections.getInternalSignalName(hsadcName, channelNum);
            if (internalSignal) {
                // This is an internal signal
                pinInfo = internalSignal;
            } else {
                // Try to get external pin info
                let peripheralPins = system.deviceData.peripherals[baseName].peripheralPins[baseName + "." + sociChannel];
                if (peripheralPins) {
                    let pinMappings = peripheralPins.pinMappings["0"];
                    if (pinMappings) {
                    pinInfo = pinMappings.designSignalName;
                    }
                }
            }
        }
    }
    return pinInfo;
}

function calculateSampleTime(inst, sociX) {
    var n = 12;
    if (inst["soc" + sociX.toString() + "UseSampleTimeCalculator"])
    {
        var Rs = inst["soc" + sociX.toString() + "InputResistance"];
        var Ron = inst["soc" + sociX.toString() + "ADCSwitchResistance"];
        var Ch = inst["soc" + sociX.toString() + "ADCSampleCapacitance"];
        var Cp = inst["soc" + sociX.toString() + "ADCParasiticCapacitance"];
        var settling_error = inst["soc" + sociX.toString() + "SettlingError"];
        var Cs = inst["soc" + sociX.toString() + "InputCapacitance"];
        var k = Math.log(Math.pow(2, n)/settling_error) -
                Math.log((Cs + Cp)/Ch)
        var tau = (Rs + Ron)*Ch + Rs*(Cs + Cp);
        var t = k*tau / 1000;

       return Math.round(t * 100) / 100;
    }

    //
    // Must return something... cannot be undefined
    //
    return 0;
}

function calculateChosenSampleTime(inst, sociX) {
    var sysclk_for_this_core = 160
    var sysclk_ns = 1000/sysclk_for_this_core;
    if (inst["soc" + sociX.toString() + "UseCalculatedSampleTime"] &&
        inst["soc" + sociX.toString() + "UseSampleTimeCalculator"])
        {
            return inst["soc" + sociX.toString() + "CalculatedSampleTime"];
        }
        else
        {
            return sysclk_ns * inst["soc" + sociX.toString() + "SampleWindow"]
        }
}

function onChangeSampleCAPReset(inst, ui) {
    for(var socIndex in device_driverlib_peripheral.ADC_SOCNumber){
        var currentSOC = device_driverlib_peripheral.ADC_SOCNumber[socIndex].name
        var soci = (currentSOC).replace(/[^0-9]/g,'')
        if (((inst.enabledSOCs).includes(currentSOC)))
        {
            if (["MSPM33C321X"].includes(Common.getDeviceName())){
                if(inst["soc" + soci.toString() + "EnableSampleCAPReset"]){
                    ui["soc" + soci.toString() + "sampleCAPResetSelect"].hidden = false;
                }
                else{
                    ui["soc" + soci.toString() + "sampleCAPResetSelect"].hidden = true;
                }
            }
        }
    }
}

function performCopy(inst, ui) {
    for(var socIndex in device_driverlib_peripheral.ADC_SOCNumber){
        var currentSOC = device_driverlib_peripheral.ADC_SOCNumber[socIndex].name
        var soci = (currentSOC).replace(/[^0-9]/g,'')
        if(((inst.enabledSOCs).includes(currentSOC)))
        {
            var inst_copy_to = inst["soc" + soci.toString() + "Name"];
            var inst_copy_from = inst["soc" + soci.toString() + "copyFrom"];
            var copyFromIndex = inst["soc" + soci.toString() + "copyFrom"].replace("SOC", "");
            if((inst_copy_to != inst_copy_from) && (inst_copy_from != ""))
            {
                //
                // Copy all SOC config
                //
                for (var configOptionIndex = 0; configOptionIndex<configNames.length; configOptionIndex++)
                {
                    var configName = configNames[configOptionIndex];
                    inst["soc" + soci.toString() + configName] = inst["soc" + copyFromIndex + configName];
                    // inst["soc" + soci.toString() + "Channel"] = inst["soc" + copyFromIndex + "Channel"]
                    // inst["soc" + soci.toString() + "SampleWindow"] = inst["soc" + copyFromIndex + "SampleWindow"]
                }
            }
        }
    }
}

// Define Pinmux object
let Pinmux = {
    calculateADCPinInfo: function(inst, sociX) {
        return calculateADCPinInfo(inst, sociX);
    },
    getDevicePinInfoDescription: function(pinInfos) {
        return getDevicePinInfoDescription(pinInfos);
    }
};

var device_adc_Ron = device_adc_char.Ron[Common.getDeviceName()]
var device_adc_Cp = device_adc_char.Cp[Common.getDeviceName()]
var device_adc_Ch = device_adc_char.Ch[Common.getDeviceName()]

/*
*  ======== validatePinmux ========
*  param inst       - module instance to be validated
*  param validation - object to hold detected validation issues
*/

function validatePinmux(inst, validation) {
    let baseName = inst.peripheral.$solution.peripheralName;
    let hsadcName = "HS" + baseName;  // HSADC0 or HSADC1
    if (inst.enabledSOCs && inst.enabledSOCs.length > 0) {
        for (let i = 0; i < hsadcSOCRange; i++) {
            let socName = "ADC_SOC_NUMBER" + i;
            if (!inst.enabledSOCs.includes(socName)) {
                continue;
            }
            let channelConfigName = "soc" + i + "Channel";
            if (!inst[channelConfigName]) {
                continue;
            }
            let sociChannel = inst[channelConfigName].replace("ADC_CH_ADCIN_", "");
            let channelNum = parseInt(sociChannel);
            // Check for NC (Not Connected) channels
            if (InternalConnections.isNCChannel(hsadcName, channelNum)) {
                validation.logError(
                    "Channel " + channelNum + " is not connected (NC) on " + hsadcName + ". " +
                    "This channel cannot be used for ADC conversion.",
                    inst,
                    channelConfigName
                );
            }
            // Check for internal channel usage and provide info
            if (InternalConnections.isInternalChannel(hsadcName, channelNum)) {
                let signalName = InternalConnections.getInternalSignalName(hsadcName, channelNum);
                if (inst.peripheral.$solution.peripheralName == "ADC0"){
                    // Special case: disChan11 Error for HSADC0 instance channel 11
                    if (signalName && signalName === "Temp Sense" && channelNum === 11) {
                        if (!inst.disChan11) {
                            validation.logError(
                                "Channel 11 on " + hsadcName + " is connected to " + signalName + ". " +
                                "Enable 'Disable Channel 11 External Pin Connection' to use Internal Connection.",
                                inst,
                                "disChan11"
                            );
                        }
                    } 
                }
                else {
                    // Special case: disChan11 Error for HSADC1 instance channel 15
                    if (signalName && signalName === "sup_atb_unbuf_out_a" && channelNum === 15) {
                        if (!inst.disChan15) {
                            validation.logError(
                            "Channel 15 on " + hsadcName + " is connected to " + signalName + ". " +
                            "Enable 'Disable Channel 15 External Pin Connection' to use Internal Connection.",
                            inst,
                            "disChan15"
                            );
                        }
                    }    
                    if (signalName && signalName === "va_msp_vref_v2i_vout_1p35v" && channelNum === 16) {
                        if (!inst.disChan16) {
                            validation.logError(
                            "Channel 16 on " + hsadcName + " is connected to " + signalName + ". " +
                            "Enable 'Disable Channel 16 External Pin Connection' to use Internal Connection.",
                            inst,
                            "disChan16"
                            );
                        } 
                    }
                }
                }
            else{ 
                if(inst.peripheral.$solution.peripheralName == "ADC1"){
                    if (inst.disChan11 && channelNum === 11) {
                        validation.logError(
                            "Channel 11 on " + hsadcName + " is connected to External Pin. " +
                            "Disable 'Disable Channel 11 External Pin Connection' to use External Pin Connection.",
                            inst,
                            "disChan11"
                        );
                    }
                }
                else {
                    if (inst.disChan15 && channelNum === 15) {
                        validation.logError(
                            "Channel 15 on " + hsadcName + " is connected to External Pin. " +
                            "Disable 'Disable Channel 15 External Pin Connection' to use External Pin Connection.",
                            inst,
                            "disChan15"
                        );
                    }
                    if (inst.disChan16 && channelNum === 16) {
                        validation.logError(
                            "Channel 16 on " + hsadcName + " is connected to External Pin. " +
                            "Disable 'Disable Channel 16 External Pin Connection' to use External Pin Connection.",
                            inst,
                            "disChan16"
                        );
                    }
                }
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

function pinmuxRequirements(inst){
    let hsadcName = inst.adcBase.replace("_BASE", "");//HSADC0
    let resources = [];
    let addedChannels = [];
    if (inst.enabledSOCs && inst.enabledSOCs.length > 0) {
        for (let i = 0; i < hsadcSOCRange; i++) {
            let socName = "ADC_SOC_NUMBER" + i;
            if (!inst.enabledSOCs.includes(socName)) {
                continue;
            }
            let channelConfigName = "soc" + i + "Channel";
            if (!inst[channelConfigName]) {
                continue;
            }
            let sociChannel = inst[channelConfigName].replace("ADC_CH_ADCIN_", "");
            let channelNum = parseInt(sociChannel);
            if (addedChannels.includes(channelNum)) {
                continue;
            }
            // Check if this channel is an internal signal 
            if (InternalConnections.isInternalChannel(hsadcName, channelNum)) {
                let signalName = InternalConnections.getInternalSignalName(hsadcName, channelNum);
                // Special case: Channel 11 on HSADC0 (Internal Connection)
                if (channelNum == 11 && !inst.disChan11 && signalName == "Temp Sense" ) {
                    addedChannels.push(channelNum);
                    // User Enabled External Pin Connection ,add pin resource
                    } 
                else {
                    // User Disabled External Pin Connection , skip pinmux but mark as added
                    addedChannels.push(channelNum);
                    continue;
                }
            }
            if(channelNum == 15){// Special case: Channel 15 on HSADC1 (Internal Connection)
                if (!inst.disChan15 ) {
                    addedChannels.push(channelNum);
                    // User Enabled External Pin Connection ,add pin resource
                } 
                else {
                    // User Disabled External Pin Connection , skip pinmux but mark as added
                    addedChannels.push(channelNum);
                    continue;
                }
            }
            if(channelNum == 16){// Special case: Channel 15 on HSADC1 (Internal Connection)
                if (!inst.disChan16 ) {
                    addedChannels.push(channelNum);
                    // User Enabled External Pin Connection ,add pin resource
                }    
                else {
                    // User Disabled External Pin Connection , skip pinmux but mark as added
                    addedChannels.push(channelNum);
                    continue;
                }
            }    
            // Check if channel is NC (Not Connected) - add pin resource
            if (InternalConnections.isNCChannel(hsadcName, channelNum)) {
                addedChannels.push(channelNum);
            }
            // Add resource for external pin channels
            resources.push({
                name: "adcPin" + channelNum,
                displayName: "HSADC Channel " + channelNum + " Pin",
                interfaceNames: [sociChannel]
            });
            addedChannels.push(channelNum);
        }
    }

    let hsadc = {
        name: "peripheral",
        displayName: "HSADC Peripheral",
        interfaceName: "ADC",
        resources: resources,
        signalTypes: {
            adcPin0: ["0"], adcPin1: ["1"], adcPin2: ["2"], adcPin3: ["3"],
            adcPin4: ["4"], adcPin5: ["5"], adcPin6: ["6"], adcPin7: ["7"],
            adcPin8: ["8"], adcPin9: ["9"], adcPin10: ["10"], adcPin11: ["11"],
            adcPin12: ["12"], adcPin13: ["13"], adcPin14: ["14"], adcPin15: ["15"],
            adcPin16: ["16"], adcPin17: ["17"], adcPin18: ["18"], adcPin19: ["19"],
            adcPin20: ["20"], adcPin21: ["21"], adcPin22: ["22"], adcPin23: ["23"],
            adcPin24: ["24"], adcPin25: ["25"], adcPin26: ["26"], adcPin27: ["27"],
            adcPin28: ["28"], adcPin29: ["29"], adcPin30: ["30"], adcPin31: ["31"]
        },        
    };
    return [hsadc];
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
function filterHardware(component)
{
    return (true);
}

/************************* IWDT Option Structures *************************/

/************************* Profiles functions *******************************/
const profilesHSADC = [
    {
        //TODO: Determine supported quick profile options
    },
];
var configNames = [
    "IndependentNameMode",
    "Channel",
    "Trigger",
    "InterruptTrigger",
    "UseSampleTimeCalculator",
    "InputCapacitance",
    "InputResistance",
    "SettlingError",
    "UseCalculatedSampleTime",
    "SampleWindow",
    "SampleTime",
];

var soc_configs = [
    {
        name        : "enabledSOCs",
        displayName : "Enable SOCs",
        description : 'Select which SOCs to enable.',
        hidden      : false,
        default     : [],
        minSelections: 0,
        options     : device_driverlib_peripheral.ADC_SOCNumber,
        onChange    : onChangeEnabledSOCs
    },
]
/*
 *  ======== onChangeProfiles ========
 *  Set options based on configuration profiles selection
 *
 */
/*
 *  ======== onChangeEnabledSEQs ========
 *  Show/hide configuration options based on enabled sequencers
 */
function onChangeEnabledSEQs(inst, ui) {
    if (!ui) return;

    for(var seqIndex in device_driverlib_peripheral.ADC_SEQNumber) {
        var currentSEQ = device_driverlib_peripheral.ADC_SEQNumber[seqIndex].name;
        var seqi = (currentSEQ).replace(/[^0-9]/g,'');
        var seqConfigs = [
            "seq" + seqi.toString() + "StartSOC",
            "seq" + seqi.toString() + "Enable",
            "seq" + seqi.toString() + "TriggerSel",
            "seq" + seqi.toString() + "DisableSampleCAPReset",
            "seq" + seqi.toString() + "sampleCAPResetSelect",
            "seq" + seqi.toString() + "SampleWindow"
            // "seq" + seqi.toString() + "UseSampleTimeCalculator",
            // "seq" + seqi.toString() + "SampleWindowCalculated",
            // "seq" + seqi.toString() + "InputCapacitance",
            // "seq" + seqi.toString() + "InputResistance",
            // "seq" + seqi.toString() + "SettlingError",
            // "seq" + seqi.toString() + "UseCalculatedSampleTime",
            // "seq" + seqi.toString() + "InputResistance",
            // "seq" + seqi.toString() + "CalculatedSampleTime",
            // "seq" + seqi.toString() + "UseCalculatedSampleTime",
            // "seq" + seqi.toString() + "ADCSwitchResistance",
            // "seq" + seqi.toString() + "ADCSampleCapacitance",
            // "seq" + seqi.toString() + "ADCParasiticCapacitance",
            // "seq" + seqi.toString() + "SettlingError"
        ];

        for (var seqConfig of seqConfigs) {
            if (ui[seqConfig] && (inst.enabledSEQs).includes(currentSEQ)) {
                ui[seqConfig].hidden = false;
            } else if (ui[seqConfig]) {
                ui[seqConfig].hidden = true;
            }
        }



        if (((inst.enabledSEQs).includes(currentSEQ))) {
            if (inst["seq" + seqi.toString() + "UseCalculatedSampleTime"] &&
               inst["seq" + seqi.toString() + "UseSampleTimeCalculator"]) {
                if (ui["seq" + seqi.toString() + "SampleWindow"]) {
                    ui["seq" + seqi.toString() + "SampleWindow"].hidden = true;
                }
                if (ui["seq" + seqi.toString() + "SampleWindowCalculated"]) {
                    ui["seq" + seqi.toString() + "SampleWindowCalculated"].hidden = false;
                }
            } else {
                if (ui["seq" + seqi.toString() + "SampleWindow"]) {
                    ui["seq" + seqi.toString() + "SampleWindow"].hidden = false;
                }
                if (ui["seq" + seqi.toString() + "SampleWindowCalculated"]) {
                    ui["seq" + seqi.toString() + "SampleWindowCalculated"].hidden = true;
                }
            }
        } else {
            if (ui["seq" + seqi.toString() + "SampleWindow"]) {
                ui["seq" + seqi.toString() + "SampleWindow"].hidden = true;
            }
            if (ui["seq" + seqi.toString() + "SampleWindowCalculated"]) {
                ui["seq" + seqi.toString() + "SampleWindowCalculated"].hidden = true;
            }
        }
    }
}
function calculateSampleWindow(inst, sociX){
    var sysclk_for_this_core = 160;
    var sysclk_ns = 1000/sysclk_for_this_core;
    return Math.round(inst["soc" + sociX.toString() + "SampleTime"]/sysclk_ns);
}
function getDeviceADCName(adcInput)
{
    if (["F2807x", "F2837xD", "F2837xS" ,"F2838x"].includes(Common.getDeviceName()))
    {
        return "ADCIN" + adcInput;
    }
    else
    {
        return adcInput;
    }
}
function getDevicePinInfoDescription(pinInfos)
{
    if (pinInfos && pinInfos.length > 0)
    {
        var pinInfoDescriptions = "";
        for (var pinInfo of pinInfos)
        {
            pinInfoDescriptions += pinInfo.PinName + ": " + pinInfo.PinSignalName + ", ";
        }
        pinInfoDescriptions = pinInfoDescriptions.substring(0, pinInfoDescriptions.length - ", ".length);
        return pinInfoDescriptions;
    }
    else
    {
        return NO_DEVICE_PIN_FOUND
    }
}

function findAllAnalogPin(selectedChannel)
{
    var selectedPinName = "";
    var selectedPinSignalName = "";
    var replaceCommaWith = "/";
    var pinInfos = [];
    for (var devicePinIndex in system.deviceData.devicePins){
        var devicePin = system.deviceData.devicePins[devicePinIndex]
        if (!isNaN(devicePin.designSignalName)){
            continue
        }
        var pin = devicePin.name.replace(/,/g, replaceCommaWith)
        var pinSignalName = devicePin.designSignalName.replace(/,/g, replaceCommaWith)
        var pinSignalNamesSplitArray = pinSignalName.split('/')
        for (var pinSignalNameSplitIndex = 0;
            pinSignalNameSplitIndex < pinSignalNamesSplitArray.length;
            pinSignalNameSplitIndex++)
        {
            var pinSignalNameSplitItem = pinSignalNamesSplitArray[pinSignalNameSplitIndex].trim();
            if (selectedChannel == pinSignalNameSplitItem)
            {
                selectedPinName = pin;
                selectedPinSignalName = pinSignalName;
                pinInfos = pinInfos.concat([{
                    PinName: selectedPinName,
                    PinSignalName : selectedPinSignalName,
                    PinDesignSignalName: devicePin.designSignalName,
                }]);
            }
        }

    }

    return pinInfos;
}

function findAnalogPin(selectedChannel)
{
    var selectedPinName = "";
    var selectedPinSignalName = "";
    var replaceCommaWith = "/";
    for (var devicePinIndex in system.deviceData.devicePins){
        var devicePin = system.deviceData.devicePins[devicePinIndex]
        if (!isNaN(devicePin.designSignalName)){
            continue
        }
        var pin = devicePin.name.replace(/,/g, replaceCommaWith)
        var pinSignalName = devicePin.designSignalName.replace(/,/g, replaceCommaWith)
        var pinSignalNamesSplitArray = pinSignalName.split('/')
        for (var pinSignalNameSplitIndex = 0;
            pinSignalNameSplitIndex < pinSignalNamesSplitArray.length;
            pinSignalNameSplitIndex++)
        {
            var pinSignalNameSplitItem = pinSignalNamesSplitArray[pinSignalNameSplitIndex].trim();
            if (selectedChannel == pinSignalNameSplitItem)
            {
                selectedPinName = pin;
                selectedPinSignalName = pinSignalName;
                return {
                    valid : true,
                    PinName: selectedPinName,
                    PinSignalName : selectedPinSignalName,
                    PinDesignSignalName : devicePin.designSignalName,
                }
            }
        }

    }

    return {
        valid : false,
        PinName: "",
        PinSignalName : "",
        PinDesignSignalName: "",
    }
}
function calculateADCPinInfo(inst,sociX){
    var adcMod = inst.adcBase.replace("_BASE", "").replace("ADC", "");
    var sociChannel = inst["soc" + sociX.toString() + "Channel"].replace("ADC_CH_ADCIN", "").replace("ADCIN", "");
    var sociChannels = sociChannel.split("_");
    var adcInputs = "";
    var adcChannelArray = []
    var adcPinInfosDescriptions = "";
    var adcPinInfosArray = [];
    for (sociChannel of sociChannels)
    {
        var adcInput = adcMod + sociChannel;
        adcChannelArray.push(adcInput);
        adcInputs += (adcInputs != "")?", ": "";
        adcInputs += adcInput;

        var adcPinInfos = findAllAnalogPin(getDeviceADCName(adcInput));
        if (["14", "15"].includes(sociChannel))
        {
            if (adcPinInfos.length <= 0)
            {
                adcPinInfos = findAllAnalogPin("ADCIN" + sociChannel);
            }
        }

        adcPinInfosDescriptions += (adcPinInfosDescriptions != "")?", ": "";
        adcPinInfosDescriptions += getDevicePinInfoDescription(adcPinInfos);
        for (var apinfo of adcPinInfos){
            adcPinInfosArray.push(apinfo.PinDesignSignalName);
        }
    }
    return {
        "adcInput": adcInput || "",
        "adcInputs": adcInputs,
        "adcPinInfosDescriptions": adcPinInfosDescriptions,
        "adcPinInfosArray": adcPinInfosArray,
        "adcChannelArray": adcChannelArray
    }
}
function onChangeSampleTimeCalculator(inst, ui){
    if (!ui) return;

    for(var socIndex in device_driverlib_peripheral.ADC_SOCNumber){
        var currentSOC = device_driverlib_peripheral.ADC_SOCNumber[socIndex].name
        var soci = (currentSOC).replace(/[^0-9]/g,'')
        var sampleCalculatorConfigs = [
            "soc" + soci.toString() + "InputCapacitance",
            "soc" + soci.toString() + "InputResistance",
            "soc" + soci.toString() + "CalculatedSampleTime",
            "soc" + soci.toString() + "UseCalculatedSampleTime",
            "soc" + soci.toString() + "ADCSwitchResistance",
            "soc" + soci.toString() + "ADCSampleCapacitance",
            "soc" + soci.toString() + "ADCParasiticCapacitance",
            "soc" + soci.toString() + "SettlingError"
        ];

        for (var sampCalcConfig of sampleCalculatorConfigs) {
            if (ui[sampCalcConfig] && inst["soc" + soci.toString() + "UseSampleTimeCalculator"]) {
                ui[sampCalcConfig].hidden = false;
            } else if (ui[sampCalcConfig]) {
                ui[sampCalcConfig].hidden = true;
            }
        }

        if (inst["soc" + soci.toString() + "UseCalculatedSampleTime"] &&
            inst["soc" + soci.toString() + "UseSampleTimeCalculator"]) {
            if ((inst.enabledSOCs).includes(currentSOC)) {
                if (ui["soc" + soci.toString() + "SampleWindow"]) {
                    ui["soc" + soci.toString() + "SampleWindow"].hidden = true;
                }
                if (ui["soc" + soci.toString() + "SampleWindowCalculated"]) {
                    ui["soc" + soci.toString() + "SampleWindowCalculated"].hidden = false;
                }
            }
        } else {
            if ((inst.enabledSOCs).includes(currentSOC)) {
                if (ui["soc" + soci.toString() + "SampleWindow"]) {
                    ui["soc" + soci.toString() + "SampleWindow"].hidden = false;
                }
                if (ui["soc" + soci.toString() + "SampleWindowCalculated"]) {
                    ui["soc" + soci.toString() + "SampleWindowCalculated"].hidden = true;
                }
            }
        }
    }
}

function calculateModuleChannelName(inst,sociX){
    return Pinmux.calculateADCPinInfo(inst,sociX).adcInputs
}
function calculateParasiticCapacitance(inst,sociX){
    var adcInput = Pinmux.calculateADCPinInfo(inst,sociX).adcInput
    var inputCp = device_adc_Cp["ADCIN" + adcInput];
    if (!inputCp)
    {
        inputCp = { CMP_DIS: ESTIMATED_CP, CMP_EN: ESTIMATED_CP }
    }
    return inputCp.CMP_EN;
}
function onChangeEnabledSOCs(inst, ui){
    if (!ui) return;

    for(var socIndex in device_driverlib_peripheral.ADC_SOCNumber){
        var currentSOC = device_driverlib_peripheral.ADC_SOCNumber[socIndex].name
        var soci = (currentSOC).replace(/[^0-9]/g,'')
        var socConfigs = [
            "soc" + soci.toString() + "Channel",
            "soc" + soci.toString() + "ModuleChannelName",
            "soc" + soci.toString() + "DevicePinName",
            "soc" + soci.toString() + "enableComparator",
        ]


        var sampleCalculatorConfigs = [
            "soc" + soci.toString() + "InputCapacitance",
            "soc" + soci.toString() + "InputResistance",
            "soc" + soci.toString() + "CalculatedSampleTime",
            "soc" + soci.toString() + "UseCalculatedSampleTime",
            "soc" + soci.toString() + "ADCSwitchResistance",
            "soc" + soci.toString() + "ADCSampleCapacitance",
            "soc" + soci.toString() + "ADCParasiticCapacitance",
            "soc" + soci.toString() + "SettlingError"
        ];

        for (var socConfig of socConfigs) {
            if (ui[socConfig] && (inst.enabledSOCs).includes(currentSOC)) {
                ui[socConfig].hidden = false;
            } else if (ui[socConfig]) {
                ui[socConfig].hidden = true;
            }
        }
    }
}
function onChangeEnabledPPBs(inst, ui){
    if (!ui) return;

    for(var ppbIndex in device_driverlib_peripheral.ADC_PPBNumber) {
        var currentPPB = device_driverlib_peripheral.ADC_PPBNumber[ppbIndex].name
        var ppbi = (currentPPB).replace(/[^0-9]/g,'')
        var ppbConfigs = [
            "ppb" + ppbi.toString() + "EventEnable",
            "ppb" + ppbi.toString() + "EventInterruptEnable",
            "ppb" + ppbi.toString() + "EnableTwosComplement",
            "ppb" + ppbi.toString() + "HighTripLimit",
            "ppb" + ppbi.toString() + "LowTripLimit",
        ]

        if ([ "MSPM33C321X"].includes(Common.getDeviceName())){
            ppbConfigs.push(
            "ppb" + ppbi.toString() + "CompSource",
            "ppb" + ppbi.toString() + "Rightshift",
            "ppb" + ppbi.toString() + "AccumulationLimit",
            )
        }

        if (["MSPM33C321X"].includes(Common.getDeviceName())){
            ppbConfigs.push("ppb" + ppbi.toString() + "EnablePPBEventCBCClear",)
        }

        for (var ppbConfig of ppbConfigs) {
            if (ui[ppbConfig] && (inst.enabledPPBs).includes(currentPPB)) {
                ui[ppbConfig].hidden = false;
            } else if (ui[ppbConfig]) {
                ui[ppbConfig].hidden = true;
            }
        }
    }
}
function onChangeUseInterrupts(inst, ui){
    if (inst.useInterrupts){
        ui.registerInterrupts.hidden = false
    }
    else{
        ui.registerInterrupts.hidden = true
    }
}
function onChangeEnabledInts(inst, ui){
    for(var intIndex in device_driverlib_peripheral.ADC_IntNumber){
        var currentINT = device_driverlib_peripheral.ADC_IntNumber[intIndex].name
        var inti = (currentINT).replace(/[^0-9]/g,'')
        var intConfigs = [
            "enableInterrupt" + inti.toString(),
            "interrupt" + inti.toString() + "SOCSource",
            "enableInterrupt" + inti.toString() + "ContinuousMode",
        ]

        if (ui)
        {
            for (var intConfig of intConfigs)
            {
                if((inst.enabledInts).includes(currentINT))
                {
                    ui[intConfig].hidden = false;
                }
                else
                {
                    ui[intConfig].hidden = true;
                }
            }
        }
    }
}
function onChangeEnabledDMAInts(inst, ui){
    for(var intIndex in device_driverlib_peripheral.ADC_IntNumber){
        var currentINT = device_driverlib_peripheral.ADC_IntNumber[intIndex].name
        var inti = (currentINT).replace(/[^0-9]/g,'')
        var intConfigs = [
            "enableDMAInterrupt" + inti.toString(),
            "dmaInterrupt" + inti.toString() + "SOCSource",
            "enableDMAInterrupt" + inti.toString() + "ContinuousMode",
        ]

        if (ui)
        {
            for (var intConfig of intConfigs)
            {
                if((inst.enabledDMAInts).includes(currentINT))
                {
                    ui[intConfig].hidden = false;
                }
                else
                {
                    ui[intConfig].hidden = true;
                }
            }
        }
    }
}

function onChangeCopy(inst, ui){
    if (!ui) return;

    for(var socIndex in device_driverlib_peripheral.ADC_SOCNumber){
        var currentSOC = device_driverlib_peripheral.ADC_SOCNumber[socIndex].name
        var soci = (currentSOC).replace(/[^0-9]/g,'')
        var copyConfigs = [
            "soc" + soci.toString() + "copyFrom",
            "soc" + soci.toString() + "copyPerform"
        ];

        for (var cpConfig of copyConfigs) {
            if (ui[cpConfig] && inst["soc" + soci.toString() + "copyUse"]) {
                ui[cpConfig].hidden = false;
            } else if (ui[cpConfig]) {
                ui[cpConfig].hidden = true;
            }
        }
    }
}
function onChangeHighPriorityMode(inst, ui){
    // NOT IN MSPM33
}
function addSOCGroup(soci){
    var eachSocConfig = [
        // ADC_setupSOC(), channel
        {
            name: "soc" + soci.toString() + "Name",
            displayName : "SOC" + soci.toString() + " Name",
            description : 'Select the application name of the channel for this SOC',
            hidden      : true,
            default     : "SOC" + soci.toString(),
        },
        {
            name: "soc" + soci.toString() + "IndependentNameMode",
            displayName : "SOC" + soci.toString() + " Independent Name Mode",
            description : 'Select the application name of the channel for this SOC and do not append the ADC instance name.',
            longDescription : 'When this option is checked, the #define generated for this SOC name will not include the ADC instance name prepend. Instead it will be a unique name. ' +
                            'Also it will trigger the generation of a new #define for the BASE of this SOC following the format "SOCx Name"_BASE ',
            hidden      : true,
            default     : false,
        },
        {
            name: "soc" + soci.toString() + "Channel",
            displayName : "SOC" + soci.toString() + " Channel",
            description : 'Select the channel for this SOC',
            hidden      : true,
            default     : device_driverlib_peripheral.ADC_Channel[0].name,
            options     : device_driverlib_peripheral.ADC_Channel,
            // TODO: decide if total or partial removal of onChangeADC
            // onChange    : onChangeADC,
        },
        {
            name: "soc" + soci.toString() + "DevicePinName",
            displayName : "SOC" + soci.toString() + " Device Pin Name",
            description : 'The device pin name and the signal name for that pin',
            hidden      : true,
            //getValue    : calculateDevicePinName,
            getValue    : (inst) => calculateDevicePinName(inst,soci),
            default     : Pinmux.getDevicePinInfoDescription(defaultADCPinInfos)
        },
        {
            name: "soc" + soci.toString() + "enableComparator",
            displayName : "SOC" + soci.toString() + " Channel Comparator Enable",
            description : "Enable or disable Channel comparator of the SOC",
            hidden      : true,
            default     : false,
        },
    ]
    if (["MSPM33C321X"].includes(Common.getDeviceName())){
        eachSocConfig = eachSocConfig.concat(
        [
            {
                name: "soc" + soci.toString() + "EnableSampleCAPReset",
                displayName : "Enable Sample Capacitor Reset",
                description : 'Enable the sample capacitor to reset after each conversion.',
                hidden      : true,
                default     : false,
                onChange    : onChangeSampleCAPReset,
            },
        ])
    }
    if (["MSPM33C321X"].includes(Common.getDeviceName())){
        eachSocConfig = eachSocConfig.concat(
        [
            {
                name: "soc" + soci.toString() + "sampleCAPResetSelect",
                displayName : "Sample Capacitor Reset Select",
                description : 'Reset Sample capacitor to either vrefhi/2 or vreflo.',
                hidden      : true,
                default     : device_driverlib_peripheral.ADC_SampleCAPResetValue[0].name,
                options     : device_driverlib_peripheral.ADC_SampleCAPResetValue,
            },
        ])
    }

    eachSocConfig = eachSocConfig.concat([
        {
            name: "GROUP_SOC" + soci.toString() + "_TRIGS",
            displayName: "SOC Triggers",
            longDescription: longDescriptionSocTriggers,
            config : SOCTrigger_configs
        },
        {
            name        : "GROUP_SOC" + soci.toString() + "_SAMPLE_TIME_CALCULATOR",
            displayName : "Sample Time Calculator",
            longDescription : longDescriptionSocSamplingTimeCalculator,
            config      : [
                {
                    name: "soc" + soci.toString() + "UseSampleTimeCalculator",
                    displayName: "Use Sample Time Calculator",
                    hidden: true,
                    default: false,
                    onChange: onChangeSampleTimeCalculator,
                },
                {
                    name: "soc" + soci.toString() + "ADCSwitchResistance",
                    displayName: "Ron ADC Sampling Switch Resistance [Ohms]",
                    hidden: true,
                    readOnly: true,
                    default: device_adc_Ron.REF,
                },
                {
                    name: "soc" + soci.toString() + "ADCSampleCapacitance",
                    displayName: "Ch ADC Sampling Capacitance [pF]",
                    hidden: true,
                    readOnly: true,
                    default: device_adc_Ch.REF,
                },
                {
                    name: "soc" + soci.toString() + "ADCParasiticCapacitance",
                    displayName: "Cp ADC Channel Parasitic Capacitance [pF]",
                    hidden: true,
                    //getValue: calculateParasiticCapacitance,
                    getValue: (inst) => calculateParasiticCapacitance(inst,soci),
                    default: defaultPinCPValue,
                },
                {
                    name: "soc" + soci.toString() + "InputCapacitance",
                    displayName: "Cs Input Capacitance [pF]",
                    hidden: true,
                    default: 0,
                },
                {
                    name: "soc" + soci.toString() + "InputResistance",
                    displayName: "Rs Input Resistance [Ohms]",
                    hidden: true,
                    default: 0,
                },
                {
                    name: "soc" + soci.toString() + "SettlingError",
                    displayName: "Settling Error [LSB]",
                    hidden: true,
                    readOnly: false,
                    default: 0.5,
                },
                {
                    name: "soc" + soci.toString() + "CalculatedSampleTime",
                    displayName: "Calculated Sample Time [ns]",
                    hidden: true,
                    //readOnly: true,
                    getValue: (inst) => calculateSampleTime(inst,soci),
                    default: 0,
                },
                {
                    name: "soc" + soci.toString() + "UseCalculatedSampleTime",
                    displayName: "Use Calculated Sample Time for this SOC",
                    hidden: true,
                    default: false,
                    onChange: onChangeSampleTimeCalculator,
                }

            ]
        },
        // ADC_setupSOC(), sampleWindow
        {
            name: "soc" + soci.toString() + "SampleWindow",
            displayName : "SOC" + soci.toString() + " Sample Window [SYSCLK counts]",
            description : 'Select the sample window (1-512) for this SOC',
            hidden      : true,
            default     : 1,
            onChange: onChangeSampleTimeCalculator,
        },
        {
            name: "soc" + soci.toString() + "SampleWindowCalculated",
            displayName : "SOC" + soci.toString() + " Sample Window [SYSCLK counts]",
            description : 'Select the sample window (1-512) for this SOC',
            hidden      : true,
            getValue    : (inst) => calculateSampleWindow(inst,soci),
            default     : 1,
        },
        {
            name: "soc" + soci.toString() + "SampleTime",
            displayName : "SOC" + soci.toString() + " Sample Time [ns]",
            description : 'Selected the sample time in ns for this SOC',
            hidden      : true,
            getValue    : (inst) => calculateChosenSampleTime(inst,soci),
            default     : ADC_SampleTime_SysClk_ns
        },
        {
            name        : "GROUP_COPY" + soci.toString(),
            displayName : "Copy Settings",
            description : '',
            config      : [
                {
                    name: "soc" + soci.toString() + "copyUse",
                    displayName: "Use Copy Feature",
                    hidden: true,
                    default: false,
                    onChange: onChangeCopy,
                },
                {
                    name: "soc" + soci.toString() + "copyFrom",
                    displayName: "Copy From",
                    description : 'The SysConfig name for the ADC SOC to copy from',
                    hidden: true,
                    default: "",
                },
                {
                    name: "soc" + soci.toString() + "copyPerform",
                    displayName: "One-shot Copy",
                    hidden: true,
                    buttonText: "Copy SOC Configuration",
                    //TBD (epmw is not prsent in mspm33)
                    // onLaunch: (inst) => {
                    //     return ({
                    //         command: "../driverlib/.meta/epwm/epwmMemory.bat",
                    //         args: ["inst.epwmMemoryFileBrowse", "$comFile"],
                    //         initialData: "",
                    //     });
                    // },
                    // onComplete: (inst, ui, result) => {
                    //     performCopy(inst, ui)
                    // },
                }

            ]
        },
    ])
    soc_configs = soc_configs.concat([{
        name: "GROUP_SOC" + soci.toString(),
        displayName: "SOC" + soci.toString(),
        description: "Start of Conversion " + soci.toString(),
        longDescription: "",
        config: eachSocConfig
    }
]);
}
// Call addSOCGroup for each SOC
for (var i = 0; i < 16; i++) {
    addSOCGroup(i);
}
var DEVICES_ADC_INSTANCES = {
    MSPM33C321X : [
        { name: "HSADC0_BASE", displayName: "HSADC0"},
        { name: "HSADC1_BASE", displayName: "HSADC1"},

    ],
}
var ADC_INSTANCE = DEVICES_ADC_INSTANCES[Common.getDeviceName()]
var maxInstances = ADC_INSTANCE.length

let config = [
    {

            name        : "adcBase",
            displayName : "HSADC Instance",
            description : 'Instance of the HSADC used.',
            hidden      : true,
            default     : ADC_INSTANCE[0].name,
            options     :  ADC_INSTANCE,

    },
    {
        name: "GROUP_BASIC",
        displayName: "Analog to digital converter (ADC) Configuration",
        longDescription: `Basic ADC Configuration Options`,
        config: [
            {
                name: "adcClockPrescaler",
                displayName: "ADC Clock Prescaler",
                description: "Clock prescaler for ADC",
                hidden: false,
                default: device_driverlib_peripheral.ADC_ClkPrescale[0].name,
                options: device_driverlib_peripheral.ADC_ClkPrescale,
            },
            {
                name: "adcEventInterrupt",
                displayName: "Add Event Handler",
                description: "Add defines for Event Handler in header file",
                hidden: false,
                default: false,
            },
            {
                name: "enableConverter",
                displayName: "Enable ADC Converter",
                description: 'Enable the analog circuitry in ADC for Conversion',
                hidden: false,
                default: true,
            }
        ]
    },
    {
        name: "GROUP_SOC",
        displayName: "SOC Configurations",
        description: "Start of Conversion Configurations",
        longDescription: "",
        config: soc_configs
    },
];
let seq_configs = [
    {
        name: "enabledSEQs",
        displayName: "Enable Sequencers",
        description: "Select which Sequencers to enable.",
        hidden: false,
        default: [],
        minSelections: 0,
        options: device_driverlib_peripheral.ADC_SEQNumber,
        onChange: onChangeEnabledSEQs
    },
    {
        name: "seqEndSOC",
        displayName: "Last SOC of all sequencers",
        description: "Select the last SOC of all sequencers",
        hidden: false,
        default: "ADC_SOC_NUMBER0",
        options: device_driverlib_peripheral.ADC_SOCNumber || [
            { name: "ADC_SOC_NUMBER0", displayName: "SOC/EOC number 0" },
            { name: "ADC_SOC_NUMBER1", displayName: "SOC/EOC number 1" },
            { name: "ADC_SOC_NUMBER2", displayName: "SOC/EOC number 2" },
            { name: "ADC_SOC_NUMBER3", displayName: "SOC/EOC number 3" },
            { name: "ADC_SOC_NUMBER4", displayName: "SOC/EOC number 4" },
            { name: "ADC_SOC_NUMBER5", displayName: "SOC/EOC number 5" },
            { name: "ADC_SOC_NUMBER6", displayName: "SOC/EOC number 6" },
            { name: "ADC_SOC_NUMBER7", displayName: "SOC/EOC number 7" },
            { name: "ADC_SOC_NUMBER8", displayName: "SOC/EOC number 8" },
            { name: "ADC_SOC_NUMBER9", displayName: "SOC/EOC number 9" },
            { name: "ADC_SOC_NUMBER10", displayName: "SOC/EOC number 10" },
            { name: "ADC_SOC_NUMBER11", displayName: "SOC/EOC number 11" },
            { name: "ADC_SOC_NUMBER12", displayName: "SOC/EOC number 12" },
            { name: "ADC_SOC_NUMBER13", displayName: "SOC/EOC number 13" },
            { name: "ADC_SOC_NUMBER14", displayName: "SOC/EOC number 14" },
            { name: "ADC_SOC_NUMBER15", displayName: "SOC/EOC number 15" }
        ]
    },
    {
        name: "seqPreemptMode",
        displayName: "Preemption mode between sequencers",
        description: "Select the preemption mode between sequencers",
        hidden: false,
        default: "ADC_PREEMPT_DISABLE",
        options: [
            { name: "ADC_PREEMPT_DISABLE", displayName: "Preemption disabled" },
            { name: "ADC_PREEMPT_NO_RESTART", displayName: "Preemption without restart" },
            { name: "ADC_PREEMPT_RESTART", displayName: "Preemption with restart" }
        ]
    }
];
// Add configuration options for each sequencer
function addSequencerConfigs() {
    // For each sequencer
    for (let seqi = 1; seqi < 5; seqi++) {
        // Add sequencer-specific configurations
        seq_configs.push({
            name: "GROUP_SEQ" + seqi.toString(),
            displayName: "Sequencer " + seqi.toString() + " Configuration",
            description: "Configuration for Sequencer " + seqi.toString(),
            collapsed: true,
            config: [
                {
                    name: "seq" + seqi.toString() + "StartSOC",
                    displayName: "Starting SOC",
                    description: "Select the starting SOC of the Sequencer",
                    hidden: true,
                    default: "ADC_SOC_NUMBER0",
                    options: device_driverlib_peripheral.ADC_SOCNumber || [
                        { name: "ADC_SOC_NUMBER0", displayName: "SOC/EOC number 0" },
                        { name: "ADC_SOC_NUMBER1", displayName: "SOC/EOC number 1" },
                        { name: "ADC_SOC_NUMBER2", displayName: "SOC/EOC number 2" },
                        { name: "ADC_SOC_NUMBER3", displayName: "SOC/EOC number 3" },
                        { name: "ADC_SOC_NUMBER4", displayName: "SOC/EOC number 4" },
                        { name: "ADC_SOC_NUMBER5", displayName: "SOC/EOC number 5" },
                        { name: "ADC_SOC_NUMBER6", displayName: "SOC/EOC number 6" },
                        { name: "ADC_SOC_NUMBER7", displayName: "SOC/EOC number 7" },
                        { name: "ADC_SOC_NUMBER8", displayName: "SOC/EOC number 8" },
                        { name: "ADC_SOC_NUMBER9", displayName: "SOC/EOC number 9" },
                        { name: "ADC_SOC_NUMBER10", displayName: "SOC/EOC number 10" },
                        { name: "ADC_SOC_NUMBER11", displayName: "SOC/EOC number 11" },
                        { name: "ADC_SOC_NUMBER12", displayName: "SOC/EOC number 12" },
                        { name: "ADC_SOC_NUMBER13", displayName: "SOC/EOC number 13" },
                        { name: "ADC_SOC_NUMBER14", displayName: "SOC/EOC number 14" },
                        { name: "ADC_SOC_NUMBER15", displayName: "SOC/EOC number 15" }
                    ]
                },
                {
                    name: "seq" + seqi.toString() + "Enable",
                    displayName: "Enable sequencer",
                    description: "Select if you want to enable sequencer or not",
                    hidden: true,
                    default: false,
                },
                {
                    name: "seq" + seqi.toString() + "TriggerSel",
                    displayName: "Trigger Source",
                    description: "Trigger source for the sequencer",
                    hidden: true,
                    default: "ADC_TRIGGER_TIELOW_SW",
                    options: [
                        { name: "ADC_TRIGGER_TIELOW_SW", displayName: "Software only" },
                        { name: "ADC_TRIGGER_GEN_SUB_0", displayName: "Generator Subsystem 0" },
                        { name: "ADC_TRIGGER_GEN_SUB_1", displayName: "Generator Subsystem 1" },
                        { name: "ADC_TRIGGER_GEN_SUB_2", displayName: "Generator Subsystem 2" },
                        { name: "ADC_TRIGGER_GEN_SUB_3", displayName: "Generator Subsystem 3" }
                    ]
                },
                {
                    name: "seq" + seqi.toString() + "DisableSampleCAPReset",
                    displayName: "Disable Sample Cap Reset",
                    description: "Disable the sample capacitor to reset after each conversion",
                    hidden: true,
                    default: false
                },
                {
                    name: "seq" + seqi.toString() + "sampleCAPResetSelect",
                    displayName: "Sample Cap Reset Select",
                    description: "Reset Sample capacitor to either vrefhi/2 or vreflo",
                    hidden: true,
                    default: "ADC_CAP_RESET_VREFLO",
                    options: [
                        { name: "ADC_CAP_RESET_VREFLO", displayName: "Sample Capacitor reset to Vreflo" },
                        { name: "ADC_CAP_RESET_VREFHIBY2", displayName: "Sample Capacitor reset to Vrefhi/2" }
                    ]
                },
                {
                    name: "seq" + seqi.toString() + "SampleWindow",
                    displayName: "Sample Window [SYSCLK counts]",
                    description: "Select the sample window (1-1472) for this Sequencer",
                    hidden: true,
                    default: 12,
                    range: [1, 1472]
                }
                // {
                //     name: "seq" + seqi.toString() + "SampleWindowCalculated",
                //     displayName: "Sample Window [SYSCLK counts]",
                //     description: "Calculated sample window for this Sequencer",
                //     hidden: true,
                //     getValue: (inst) => calculateSampleWindow(inst, seqi),
                //     default: 1,
                //     readOnly: true
                // },
                // {
                //     name: "seq" + seqi.toString() + "UseSampleTimeCalculator",
                //     displayName: "Use Sample Time Calculator",
                //     description: "Use the sample time calculator for this Sequencer",
                //     hidden: true,
                //     default: false,
                //     onChange: onChangeSampleTimeCalculator
                // },
                // {
                //     name: "seq" + seqi.toString() + "InputCapacitance",
                //     displayName: "Input Capacitance [pF]",
                //     description: "Input Capacitance in pF",
                //     hidden: true,
                //     default: 0
                // },
                // {
                //     name: "seq" + seqi.toString() + "InputResistance",
                //     displayName: "Input Resistance [Ohms]",
                //     description: "Input Resistance in Ohms",
                //     hidden: true,
                //     default: 0
                // },
                // {
                //     name: "seq" + seqi.toString() + "SettlingError",
                //     displayName: "Settling Error [LSB]",
                //     description: "Settling Error in LSB",
                //     hidden: true,
                //     default: 0.5
                // },
                // {
                //     name: "seq" + seqi.toString() + "CalculatedSampleTime",
                //     displayName: "Calculated Sample Time [ns]",
                //     description: "Calculated Sample Time in ns",
                //     hidden: true,
                //     getValue: (inst) => calculateSampleTime(inst, seqi),
                //     default: 0,
                //     readOnly: true
                // },
                // {
                //     name: "seq" + seqi.toString() + "UseCalculatedSampleTime",
                //     displayName: "Use Calculated Sample Time",
                //     description: "Use the calculated sample time for this Sequencer",
                //     hidden: true,
                //     default: false,
                //     onChange: onChangeSampleTimeCalculator
                // }
            ]
        });
    }
}

// Call the function to add sequencer-specific configurations
addSequencerConfigs();
// Define the module configuration
config = config.concat([
    {
        name: "GROUP_SEQ",
        displayName: "Sequencer Configurations",
        description: "ADC Sequencer Configurations",
        longDescription: "Configure the ADC Sequencers",
        config: seq_configs
    }
])

var int_configs = [
    // ADC_setInterruptPulseMode(), pulseMode
    {
        name        : "interruptPulseMode",
        displayName : "ADC Interrupt Pulse Mode",
        description : 'Interrupt Pulse Mode for ADC EOC (end of conversion).',
        hidden      : false,
        default     : device_driverlib_peripheral.ADC_PulseMode[0].name,
        options     : device_driverlib_peripheral.ADC_PulseMode,
    }
]
// if (["MSPM33C321X"].includes(Common.getDeviceName()))
//     {
//         // ADC_setInterruptCycleOffset(), cycleOffset
//         int_configs = int_configs.concat([
//             {
//                 name        : "interruptCycleOffset",
//                 displayName : "ADC Interrupt Cycle Offset",
//                 description : 'Cycle offset between the negative edge of a sample ' +
//                               'pulse and an early interrupt pulse being generated.',
//                 hidden      : false,
//                 default     : 0
//             }
//         ]);
//     }

    int_configs = int_configs.concat([
        {
            name        : "enabledInts",
            displayName : "Enable ADC Interrupts",
            description : 'Select which INTs to enable.',
            hidden      : false,
            default     : [],
            minSelections: 0,
            options     : device_driverlib_peripheral.ADC_IntNumber,
            onChange    : onChangeEnabledInts
        },
    ])
    var interruptSourceOption = device_driverlib_peripheral.ADC_SOCNumber;
    // if (["MSPM33C321X"].includes(Common.getDeviceName())){
    //     interruptSourceOption = device_driverlib_peripheral.ADC_IntTrigger;
    // }
        for(var intIndex in device_driverlib_peripheral.ADC_IntNumber){
            var currentINT = device_driverlib_peripheral.ADC_IntNumber[intIndex].name
            var inti = (currentINT).replace(/[^0-9]/g,'')
            int_configs = int_configs.concat([
                {
                    name: "GROUP_INT" + inti.toString(),
                    displayName: "INT" + inti.toString(),
                    description: "ADC Interrupt " + inti.toString(),
                    longDescription: "",
                    config: [
                        // ADC_enableInterrupt() / ADC_disableInterrupt(), adcIntNum
                        {
                            name: "enableInterrupt" + inti.toString(),
                            displayName : "Enable ADC Interrupt " + inti.toString(),
                            description : 'Enable the interrupt' + inti.toString() + ' for this ADC',
                            hidden      : true,
                            default     : false
                        },
                        // ADC_setInterruptSource(), socNumber
                        {
                            name: "interrupt" + inti.toString() + "SOCSource",
                            displayName : "Interrupt " + inti.toString() + " SOC Source",
                            description : 'Select the source for interrupt' + inti.toString() + ' of this ADC',
                            hidden      : true,
                            default     : interruptSourceOption[0].name,
                            options     : interruptSourceOption,
                        },
                        // ADC_enableContinuousMode()
                        {
                            name: "enableInterrupt" + inti.toString() + "ContinuousMode",
                            displayName : "Continuous Interrupt Mode",
                            description : 'When this is enabled, every EOC signal generates an interrupt regardless of whether the interrupt flag has been cleared or not. When this is disabled, interrupts are suppressed while there is an existing interrupt flag.',
                            hidden      : true,
                            default     : false
                        },
                    ]
                }
            ])
        }

    if (["MSPM33C321X"].includes(Common.getDeviceName())){
    int_configs = int_configs.concat([
        {
            name        : "enabledDMAInts",
            displayName : "Enable ADC DMA Interrupts",
            description : 'Select which INTs to enable.',
            hidden      : false,
            default     : [],
            minSelections: 0,
            options     : device_driverlib_peripheral.ADC_IntNumber,
            onChange    : onChangeEnabledDMAInts
        },
    ])

    // interruptSourceOption = device_driverlib_peripheral.ADC_IntTrigger;
        for(var intIndex in device_driverlib_peripheral.ADC_IntNumber){
            var currentINT = device_driverlib_peripheral.ADC_IntNumber[intIndex].name
            var inti = (currentINT).replace(/[^0-9]/g,'')
            int_configs = int_configs.concat([
                {
                    name: "GROUP_DMAINT" + inti.toString(),
                    displayName: "DMAINT" + inti.toString(),
                    description: "ADC DMA Interrupt " + inti.toString(),
                    longDescription: "",
                    config: [
                        // ADC_enableDMAInterrupt() / ADC_disableDMAInterrupt(), adcIntNum
                        {
                            name: "enableDMAInterrupt" + inti.toString(),
                            displayName : "Enable ADC DMA Interrupt " + inti.toString(),
                            description : 'Enable the interrupt for DMA' + inti.toString() + ' for this ADC',
                            hidden      : true,
                            default     : false
                        },
                        // ADC_setDMAInterruptSource(), socNumber
                        {
                            name: "dmaInterrupt" + inti.toString() + "SOCSource",
                            displayName : "DMA Interrupt " + inti.toString() + " SOC Source",
                            description : 'Select the source for DMA interrupt' + inti.toString() + ' of this ADC',
                            hidden      : true,
                            default     : interruptSourceOption[0].name,
                            options     : interruptSourceOption,
                        },
                        // ADC_enableDMAContinuousMode()
                        {
                            name: "enableDMAInterrupt" + inti.toString() + "ContinuousMode",
                            displayName : "Continuous DMA Interrupt Mode",
                            description : 'When this is enabled, every EOC signal generates an interrupt regardless of whether the DMA interrupt flag has been cleared or not. When this is disabled, interrupts are suppressed while there is an existing DMA interrupt flag.',
                            hidden      : true,
                            default     : false
                        },
                    ]
                }
            ])
        }
    }
    config = config.concat([
        {
            name: "GROUP_INT",
            displayName: "ADC INT Configurations",
            description: "Interrupt Configurations",
            longDescription: "",
            config: int_configs
        }
    ])
var registerIntLongDesc = `
Each ADC module has 4 configurable ADC interrupts. These interrupts can be triggered by any of the 16 EOC
signals. The flag bit for each ADCINT can be read directly to determine if the associated SOC is complete or the
interrupt can be passed on to the PIE.

![an offline image](../../driverlib/.meta/adc/docs/adcEocInts.png "ADC EOC Interrupts")

One PIE interrupt is shared between all the PPBs for a given ADC module.

![an offline image](../../driverlib/.meta/adc/docs/adcPPBInt.png "ADC PPB Interrupt Event")

`;
var ppb_configs = []

ppb_configs = ppb_configs.concat([
    {
        name        : "enabledPPBs",
        displayName : "Enable PPBs",
        description : 'Select which PPBs to enable.',
        hidden      : false,
        default     : [],
        minSelections: 0,
        options     : device_driverlib_peripheral.ADC_PPBNumber,
        onChange    : onChangeEnabledPPBs
    },
])

let ppbEvents = [
    {name: "ADC_EVT_TRIPHI", displayName: "ADC Event TRIP HIGH"},
    {name: "ADC_EVT_TRIPLO", displayName: "ADC Event TRIP LOW"},
]
if (["MSPM33C321X"].includes(Common.getDeviceName())){
    ppbEvents = [
        {name: "ADC_EVT_TRIPHI", displayName: "ADC Event TRIP HIGH"},
        {name: "ADC_EVT_TRIPLO", displayName: "ADC Event TRIP LOW"},
        {name: "ADC_EVT_INLIMIT", displayName: "ADC Event Within Trip Limit"},
    ]
}
for(var ppbIndex in device_driverlib_peripheral.ADC_PPBNumber)
{
    var currentPPB = device_driverlib_peripheral.ADC_PPBNumber[ppbIndex].name
    var ppbi = (currentPPB).replace(/[^0-9]/g,'')
    var ppbx_configs = [
        {
            name: "ppb" + ppbi.toString() + "Name",
            displayName : "Name",
            description : 'Select the PPB name for this PPB',
            hidden      : true,
            default     : "PPB" + ppbi.toString()
        },
        {
            name: "ppb" + ppbi.toString() + "IndependentNameMode",
            displayName : "Independent Name Mode",
            description : 'Select the application name of the PPB and do not append the ADC instance name.',
            longDescription : 'When this option is checked, the #define generated for this PPB name will not include the ADC instance name prepend. Instead it will be a unique name. ' +
                              'Also it will trigger the generation of a new #define for the SOC of this PPB following the format',
            hidden      : true,
            default     : false,
        },
        // ADC_setupPPB(), socNumber
        {
            name: "ppb" + ppbi.toString() + "SOCNumber",
            displayName : "SOC Number",
            description : 'Select the SOC for this PPB',
            hidden      : true,
            default     : device_driverlib_peripheral.ADC_SOCNumber[0].name,
            options     : device_driverlib_peripheral.ADC_SOCNumber,
        },

        // ADC_enablePPBEvent() / ADC_disablePPBEvent, evtFlags
        {
            name: "ppb" + ppbi.toString() + "EventEnable",
            displayName :  "Enable Events",
            description : 'Select the EVENTs to enable for this PPB',
            hidden      : true,
            minSelections: 0,
            default     : [],
            options     : ppbEvents
        },
        // ADC_enablePPBEventInterrupt() / ADC_disablePPBEventInterrupt(), intFlags
        {
            name: "ppb" + ppbi.toString() + "EventInterruptEnable",
            displayName : "Enable Interrupt Events",
            description : 'Select the Interrupt EVENTs to enable for this PPB',
            hidden      : true,
            minSelections: 0,
            default     : [],
            options     : ppbEvents
        },


        // ADC_enablePPBTwosComplement() / ADC_disablePPBTwosComplement()
        // {
        //     name: "ppb" + ppbi.toString() + "EnableTwosComplement",
        //     displayName : "Invert Output (Enable Two's Complement)",
        //     description : 'Select to enable the Two\'s complement mode for this PPB',
        //     hidden      : true,
        //     default     : false
        // },
        // ADC_setPPBTripLimits(), tripHiLimit
        {
            name: "ppb" + ppbi.toString() + "HighTripLimit",
            displayName : "High Trip Limit",
            description : 'High Trip limit for this PPB',
            hidden      : true,
            default     : 0
        },
        // ADC_setPPBTripLimits() , tripLoLimit
        {
            name: "ppb" + ppbi.toString() + "LowTripLimit",
            displayName : "Low Trip Limit",
            description : 'Low Trip limit for this PPB',
            hidden      : true,
            default     : 0
        },
    ]
    if (["MSPM33C321X"].includes(Common.getDeviceName())) {
        ppbx_configs = ppbx_configs.concat([

            //ADC_setPPBCountLimit(), limit
            {
                name: "ppb" + ppbi.toString() + "AccumulationLimit",
                displayName: "Accumulation Limit",
                description:
                "Select target sample counts to generate over-sampling interrupt",
                longDescription: "Accumulation Limit defines the number of ADC conversions to accumulate before partial sum is automatically loaded to the sum registers. When the partial conversion count equals the Accumulation Limit, the PPB loads the values of the respective partial result registers into the final result registers and generates an oversampling event interrupt",
                hidden: true,
                default: device_driverlib_peripheral.ADC_PPB_Oversampling[0].name,
                options: device_driverlib_peripheral.ADC_PPB_Oversampling,
            },

            //ADC_selectPPBCompareSource(), compSrc
            {
                name: "ppb" + ppbi.toString() + "CompSource",
                displayName: "Compare Source",
                description: "Select the compare source for this PPB",
                hidden: true,
                default: device_driverlib_peripheral.ADC_PPBCompSource[0].name,
                options: device_driverlib_peripheral.ADC_PPBCompSource,
            },
            // ADC_setPPBShiftValue, shiftVal
            {
                name: "ppb" + ppbi.toString() + "Rightshift",
                displayName: "Number of Bits to Right Shift PSUM",
                description: "Select number of bits to right shift PSUM before loading to final PPB SUM",
                hidden: true,
                default: 0,
                range: [0, 7]
            },
        ]);
    }


    ppb_configs = ppb_configs.concat([{
        name: "GROUP_PPB" + ppbi.toString(),
        displayName: "PPB" + ppbi.toString(),
        description: "Post Processing Block " + ppbi.toString(),
        longDescription: "",
        config: ppbx_configs
    }])

}

config = config.concat([


    {
        name: "GROUP_PPB",
        displayName: "PPB Configurations",
        description: "Post Processing Blocks Configurations",
        longDescription: "",
        config: ppb_configs
    },
])

var globalConfig = [

    {
        name: "sampleWindowSYSCLK",
        deprecated: true,
        displayName: "SYSCLK [MHz] for Sample Window Calculation",
        description: "This is the SYSCLK value assumed for sample window calculation and block diagram generation",
        default: ADC_SysClk_MHz,
        onChange : (inst, ui) => {

        }
    },
    {
        name: "synchronousModeCheck",
        displayName: "Check for Valid Synchronous Mode",
        description: "Check to see if all SOCs configured for the same trigger have the same sample window",
        default: false
    }
]

config = config.concat([
    {
        name: "eventConfig",
        displayName: "Event Configuration",
        description: "Event Subscriber/Publisher Configurations",
        longDescription: `Specifies the Timer Events that are to trigger an Event Condition and where the events are to be published`,
        collapsed: true,
        config: [
            {
                name: "ADCevent1PublisherChannel",
                displayName: "Event 1 Publisher Channel ID",
                description: "Specifies the Channel For Event 1",
                longDescription: `Specifies the Channel to which ADC Events in Event 1 will be published for use by other modules`,
                default: 0,
                options: (inst) => {
                    return EVENT.getPublisherOptions(inst);
                },
            },
            {
                name: "ADCevent2PublisherChannel",
                displayName: "Event 2 Publisher Channel ID",
                description: "Specifies the Channel For Event 2",
                longDescription: `Specifies the Channel to which ADC Events in Event 2 will be published for use by other modules`,
                default: 0,
                options: (inst) => {
                    return EVENT.getPublisherOptions(inst);
                },
            },
            {
                name: "ADCevent3PublisherChannel",
                displayName: "Event 3 Publisher Channel ID",
                description: "Specifies the Channel For Event 3",
                longDescription: `Specifies the Channel to which ADC Events in Event 3 will be published for use by other modules`,
                default: 0,
                options: (inst) => {
                    return EVENT.getPublisherOptions(inst);
                },
            },
            {
                name: "ADCevent4PublisherChannel",
                displayName: "Event 4 Publisher Channel ID",
                description: "Specifies the Channel For Event 4",
                longDescription: `Specifies the Channel to which ADC Events in Event 4 will be published for use by other modules`,
                default: 0,
                options: (inst) => {
                    return EVENT.getPublisherOptions(inst);
                },
            },
            {
                name: "ADCsubscriber1Channel",
                displayName: "Subscriber Port 1 Channel Selection",
                description: "Specifies the channel for a subscriber port 1",
                longDescription: `Specifies the event subscriber channel`,
                default: 0,
                options: (inst) => {
                    return EVENT.getSubscriberOptions(inst);
                },
                // getDisabledOptions: (inst) => {
                //     return EVENT.getSubscriberDisabledOptions(inst);
                // },
            },
            {
                name: "ADCsubscriber2Channel",
                displayName: "Subscriber Port 2 Channel Selection",
                description: "Specifies the channel for a subscriber port 2",
                longDescription: `Specifies the event subscriber channel`,
                default: 0,
                options: (inst) => {
                    return EVENT.getSubscriberOptions(inst);
                },
                // getDisabledOptions: (inst) => {
                //     return EVENT.getSubscriberDisabledOptions(inst);
                // },
            },
            {
                name: "ADCsubscriber3Channel",
                displayName: "Subscriber Port 3 Channel Selection",
                description: "Specifies the channel for a subscriber port 3",
                longDescription: `Specifies the event subscriber channel`,
                default: 0,
                options: (inst) => {
                    return EVENT.getSubscriberOptions(inst);
                },
                // getDisabledOptions: (inst) => {
                //     return EVENT.getSubscriberDisabledOptions(inst);
                // },
            },
            {
                name: "ADCsubscriber4Channel",
                displayName: "Subscriber Port 4 Channel Selection",
                description: "Specifies the channel for a subscriber port 4",
                longDescription: `Specifies the event subscriber channel`,
                default: 0,
                options: (inst) => {
                    return EVENT.getSubscriberOptions(inst);
                },
                // getDisabledOptions: (inst) => {
                //     return EVENT.getSubscriberDisabledOptions(inst);
                // },
            },
            // {
            //     name: "eventInfo",
            //     description: "information about configured events that's passed on to the EVENT module",
            //     default: [""],
            //     hidden: true,
            //     options: (inst) => {
            //         let options = [{name: ""}]
            //         if(inst.event1PublisherChannel != 0){
            //             options.push({name: inst.event1PublisherChannel.toString() + "pub"});
            //         }
            //         if(inst.event2PublisherChannel != 0){
            //             options.push({name: inst.event2PublisherChannel.toString() + "pub"});
            //         }
            //         if((inst.subscriberPort != "Disabled") && (inst.subscriberChannel != 0)){
            //             options.push({name: inst.subscriberChannel.toString() + "sub"});
            //         }
            //         return options;
            //     },
            //     getValue: (inst) => {
            //         let result = [];
            //         if(inst.event1PublisherChannel != 0){
            //             result.push(inst.event1PublisherChannel.toString() + "pub");
            //         }
            //         if(inst.event2PublisherChannel != 0){
            //             result.push(inst.event2PublisherChannel.toString() + "pub");
            //         }
            //         if((inst.subscriberPort != "Disabled") && (inst.subscriberChannel != 0)){
            //             result.push(inst.subscriberChannel.toString() + "sub");
            //         }
            //         if(result.length == 0){
            //             result = [""];
            //         }
            //         return result;
            //     }
            // }

        ]
    }
])

config = config.concat([
    {
        name        : "disChan11",
        displayName : "Disable Channel 11 External Pin Connection",
        description : 'Disable the External Pin connection on Channel 11 to use Internal Temperature Sensor.',
        longDescription : `On HSADC0, Channel 11 is internally connected to the Temperature Sensor. If you need to use Channel 11 as an Internal Connection, enable this option to disconnect the external pin connection and allow  internal Temperature Sensor`,
        hidden      : false,  
        default     : false,
    },
    {
        name        : "disChan15",
        displayName : "Disable Channel 15 External Pin Connection",
        description : 'Disable the External Pin connection on Channel 15 to use Internal Connection "sup_atb_unbuf_out_a".',
        longDescription : `On HSADC1, Channel 15 is internally connected to the "sup_atb_unbuf_out_a". If you need to use Channel 15 as an Internal Connection, enable this option to disconnect the external pin connection and allow  internal Connection "sup_atb_unbuf_out_a"`,
        hidden      : false,  
        default     : false,
    },
    {
        name        : "disChan16",
        displayName : "Disable Channel 16 External Pin Connection",
        description : 'Disable the External Pin connection on Channel 16 to use Internal Connection "va_msp_vref_v2i_vout_1p35v".',
        longDescription : `On HSADC1, Channel 16 is internally connected to the "va_msp_vref_v2i_vout_1p35v". If you need to use Channel 16 as an Internal Connection, enable this option to disconnect the external pin connection and allow  internal Connection "va_msp_vref_v2i_vout_1p35v"`,
        hidden      : false,  
        default     : false,
    },
])

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base module configuration
 */
let devSpecific = {
    config: config,
    maxInstances: maxInstances,
    validatePinmux: validatePinmux,
    pinmuxRequirements: pinmuxRequirements,
    modules: setRequiredModules,
};

function setRequiredModules(inst){
    let theModules = ["Board", "SYSCTL"]
    let kwargs = theModules;
    let modArray = [];

    for (let args = kwargs.length - 1; args >= 0; args--) {
        let modPath = kwargs[args];
        if (modPath.indexOf('/') == -1) {
            modPath = "/ti/driverlib/" + modPath;
        }
        modArray.push({
            name      : modPath.substring(modPath.lastIndexOf('/') + 1),
            moduleName: modPath,
            hidden    : true
        });
    }

    return modArray
};

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic module module to
 *  allow us to augment/override as needed for the MSP432.
 */
function extend(base)
{
    /* merge and overwrite base module attributes */
    Object.assign(base, devSpecific);

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
