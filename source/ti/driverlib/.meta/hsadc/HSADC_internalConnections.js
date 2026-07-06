let Common = system.getScript("/ti/driverlib/Common.js");
let HSADC_internalSignals = {
    MSPM33C321X: {
        HSADC0: {
            CH11: "Temp Sense",
            CH22: "sup_atb_buf_out_a",
            CH23: "va_msp_vref_v2i_vout_1p35v",
            CH24: "NC",
            CH25: "NC",
            CH26: "NC",
            CH27: "NC",
            CH28: "NC",
            CH29: "NC",
            CH30: "VBAT Supply Monitor",
            CH31: "VDD Supply Monitor"
        },
        HSADC1: {
            CH15: "sup_atb_unbuf_out_a",
            CH16: "va_msp_vref_v2i_vout_1p35v",
            CH17: "NC",
            CH18: "NC",
            CH19: "NC",
            CH20: "NC",
            CH21: "NC",
            CH22: "NC",
            CH23: "NC",
            CH24: "NC",
            CH25: "NC",
            CH26: "NC",
            CH27: "NC",
            CH28: "NC",
            CH29: "NC",
            CH30: "VBAT Supply Monitor",
            CH31: "VDD Supply Monitor"
        }
    }
}

/*
*  ======== isInternalChannel ========
*  Check if a channel is internal for a specific HSADC instance
*
*  param hsadcInstance - "HSADC0" or "HSADC1"
*  param channelNum    - channel number (0-31)
*
*  returns true if internal, false if external pin
*/
function isInternalChannel(hsadcInstance, channelNum) {
    let deviceFamily = Common.getDeviceName();
    let internalSignals = HSADC_internalSignals[deviceFamily];
    if (!internalSignals || !internalSignals[hsadcInstance]) {
        return false;
    }
    let channelKey = "CH" + channelNum;
    return internalSignals[hsadcInstance].hasOwnProperty(channelKey);
}

/*
*  ======== isNCChannel ========
*  Check if a channel is Not Connected (NC) for a specific HSADC instance
*
*  param hsadcInstance - "HSADC0" or "HSADC1"
*  param channelNum    - channel number (0-31)
*
*  returns true if NC, false otherwise
*/
function isNCChannel(hsadcInstance, channelNum) {
    let deviceFamily = Common.getDeviceName();
    let internalSignals = HSADC_internalSignals[deviceFamily];
    if (!internalSignals || !internalSignals[hsadcInstance]) {
        return false;
    }
    let channelKey = "CH" + channelNum;
    let signal = internalSignals[hsadcInstance][channelKey];
    return signal === "NC";
}

/*
*  ======== getInternalSignalName ========
*  Get the internal signal name for a channel
*
*  param hsadcInstance - "HSADC0" or "HSADC1"
*  param channelNum    - channel number (0-31)
*
*  returns signal name string or null if not internal
*/
function getInternalSignalName(hsadcInstance, channelNum) {
    let deviceFamily = Common.getDeviceName();
    let internalSignals = HSADC_internalSignals[deviceFamily];
    if (!internalSignals || !internalSignals[hsadcInstance]) {
        return null;
    }
    let channelKey = "CH" + channelNum;
    return internalSignals[hsadcInstance][channelKey] || null;
}

exports = {
    HSADC_internalSignals: HSADC_internalSignals,
    isInternalChannel: isInternalChannel,
    isNCChannel: isNCChannel,
    getInternalSignalName: getInternalSignalName,    
}