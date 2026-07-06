/*
 * Copyright (c) 2026 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== QSPIMSPM0.syscfg.js ========
 */

"use strict";

/* get Common /ti/driverlib utility functions */
let Common = system.getScript("/ti/driverlib/Common.js");

/************************Interrupt Functions **********************************/

/************************Interrupt Functions **********************************/
function getIntOptions(inst)
{
    let allOptions = [
        {name: "DMA_DONE_TX", displayName: "DMA done on transit"},
        {name: "DMA_DONE_RX", displayName: "DMA done on receive"},
        {name: "IDLE", displayName: "Idle"},
        {name: "TX_EMPTY", displayName: "Transmit FIFO empty"},
        {name: "TX", displayName: "Transmit"},
        {name: "RX", displayName: "Receive"},
        {name: "PARITY_ERROR", displayName: "Parity error"},
        {name: "RX_OVERFLOW", displayName: "Receive FIFO overflow"},
        {name: "RX_FULL", displayName: "Receive FIFO full"},
        {name: "TX_UNDERFLOW", displayName: "Transmit FIFO underflow"},
    ];

    return allOptions;
}

function onFrameFormatEnable(inst, ui){

    /* QSPI only supports Controller mode */
    if((inst.frameFormat == "MOTO4") || (inst.frameFormat == "MOTO3")){
        ui["polarityPhase"].hidden = false
    }
    else {
        ui["polarityPhase"].hidden = true
    }

    if (inst.frameFormat == "MOTO3")
    {
        ui["chipSelect"].hidden = true
        ui["defaultChipSelect"].hidden = true
        ui["peripheralChipSelect"].hidden = true
    }
    else
    {
        /* QSPI only supports Controller mode */
        ui["chipSelect"].hidden = false
        /* Only show defaultChipSelect if > 1 CS has been selected */
        ui["defaultChipSelect"].hidden = !(inst.chipSelect.length > 1)
        ui["peripheralChipSelect"].hidden = true
    }
}


function onParityEnable(inst, ui){
    if (inst.parity != "NONE") {
        ui["parityDirection"].hidden = false
    }
    else
    {
        ui["parityDirection"].hidden = true
    }
}

/* QSPI does not support Command Data Mode - removed onCDModeEnable and onCDModeCustom functions */

/************************* General functions *******************************/
function hasMuxablePins(interfaceName, pinName) {
    /*
     * _.some returns true if the function supplied in the second parameter
     *  returns true for any element)
     */

    const interfacePin = system.deviceData.interfaces[interfaceName].interfacePins[pinName];
    return _.some(interfacePin.pinMappings, (peripheralPin) => peripheralPin.pinMappings.length !== 0);
}

const QSPICSOptions = [
    {name: "0", displayName: "CS0"},
    {name: "1", displayName: "CS1"},
    {name: "2", displayName: "CS2"},
    {name: "3", displayName: "CS3"},
];

function getQSPICSDisabledOptions(inst)
{
    let disabledOptions = [];
    /* QSPI does not support CD Mode - no CS restrictions */
    return disabledOptions;
}

function getQSPIDefaultCSDisabledOptions(inst)
{
    let disabledOptions = [];

    for (let csOption in QSPICSOptions)
    {
        if (!inst.chipSelect.includes(csOption))
        {
            disabledOptions.push({name: csOption,
                                  displayName: "CS" + csOption,
                                  reason: "CS" + csOption + " has not been selected as CS"});
        }
    }

    return disabledOptions;
}

/************************* Profiles functions *******************************/
/*
 *  ======== onChangeQSPIProfile ========
 *  Called when QSPI Profile is changed.
 *  Pre-configures QSPI according to profiles
 *
 */
    let defaultDMAConfig = {
        enabledDMAEvent1Triggers : "None",
        enabledDMAEvent2Triggers : "None",
        enableDMAEvent1 : false,
        enableDMAEvent2 : false,
    }
    /* QSPI Profile 1: Standard Dual Mode (PICO/POCI) at 32 MHz */
    let configProfile1 = {
        name : "CONFIG_PROFILE_1",
        frameFormat : "MOTO4",
        polarityPhase : "0_0",
        dataSize: 8,
        chipSelect: ["0"],
        targetBitRate : 32000000,
        calculatedBitRate: 0,
        rxFifoThreshold: "DL_QSPI_RX_FIFO_LEVEL_1_2_FULL",
        txFifoThreshold: "DL_QSPI_TX_FIFO_LEVEL_1_2_EMPTY",
        direction: "PICO_POCI",
        parity: "NONE",
        enableInternalLoopback: false,
        delayedSampling: 0,
        chipSelectAssertDelay: "DL_QSPI_CHIP_SELECT_ASSERT_DELAY_0P5",
        chipSelectDeAssertDelay: "DL_QSPI_CHIP_SELECT_DEASSERT_DELAY_0P5",
        dataSampleDelay: "DL_QSPI_DATA_SAMPLE_DELAY_0",
        enabledInterrupts : [],
        interruptPriority : "DEFAULT",
        ...defaultDMAConfig,
    };
    /* QSPI Profile 2: Quad Mode (PICO/POCI/IO2/IO3) at 32 MHz */
    let configProfile2 = {
        name : "CONFIG_PROFILE_2",
        frameFormat : "MOTO4",
        polarityPhase : "0_0",
        dataSize: 8,
        chipSelect: ["0"],
        targetBitRate : 32000000,
        calculatedBitRate: 0,
        rxFifoThreshold: "DL_QSPI_RX_FIFO_LEVEL_1_2_FULL",
        txFifoThreshold: "DL_QSPI_TX_FIFO_LEVEL_1_2_EMPTY",
        direction: "PICO_POCI_IO2_IO3",
        parity: "NONE",
        enableInternalLoopback: false,
        delayedSampling: 0,
        chipSelectAssertDelay: "DL_QSPI_CHIP_SELECT_ASSERT_DELAY_0P5",
        chipSelectDeAssertDelay: "DL_QSPI_CHIP_SELECT_DEASSERT_DELAY_0P5",
        dataSampleDelay: "DL_QSPI_DATA_SAMPLE_DELAY_0",
        enabledInterrupts : [],
        interruptPriority : "DEFAULT",
        ...defaultDMAConfig,
    };
const profilesQSPI = [
    configProfile1,
    configProfile2,
];

function onChangeQSPIProfile(inst, ui) {
    if (inst.profile != "CUSTOM")
    {
        let selectedProfileArray = profilesQSPI.filter(obj =>
                                        {return obj.name === inst.profile});
        if (selectedProfileArray.length != 0)
        {
            const selectedProfile = Object.assign({}, selectedProfileArray[0]);
            //selectedProfile = selectedProfile[0];
            delete selectedProfile.name;
            Object.assign(inst, selectedProfile)
        }
        updateGUIbasedonConfig(inst, ui);
    }
}

function updateGUIbasedonConfig(inst, ui)
{
    updateGUIChangeMode(inst, ui)
    onFrameFormatEnable(inst, ui)
    onParityEnable(inst, ui)
    /* QSPI does not support CD Mode */
}

/* Functions that update GUI based on Configuration */
function updateGUIChangeMode(inst, ui)
{
    /* QSPI only supports Controller mode */
    /* Reveal Controller specific functions */
    ui["delayedSampling"].hidden = false
    ui["delayedSamplingCalc"].hidden = false
    ui["enableInternalLoopback"].hidden = false
    ui["targetBitRate"].hidden = false
    ui["calculatedBitRate"].hidden = false
    ui["calculatedError"].hidden = false

    /* Hide Peripheral specific functions */
    ui["clearRXCounterOnCSIdle"].hidden = true

    if (inst.frameFormat == "MOTO3")
    {
        ui["chipSelect"].hidden = true
        ui["defaultChipSelect"].hidden = true
        ui["peripheralChipSelect"].hidden = true
    }
    else
    {
        /* QSPI only supports Controller mode */
        ui["chipSelect"].hidden = false
        /* Only show defaultChipSelect if > 1 CS has been selected */
        ui["defaultChipSelect"].hidden = !(inst.chipSelect.length > 1)
        ui["peripheralChipSelect"].hidden = true
    }
    /* QSPI does not support CD Mode */
}

/* When target baud rate is changed, set the calculated baud rate back to 0 */
function updateGUIbasicBitRateCalc(inst, ui) {
    inst.calculatedBitRate = 0;
}

function updateGUIInterrupts(inst, ui){
    ui.interruptPriority.hidden = (inst.enabledInterrupts.length === 0);
}

function onChangeSetCustomProfile(inst,ui){
    inst.profile = "CUSTOM";
}

/************************* On Change functions *******************************/
/* onChange functions  */
function onChangeEnableController(inst, ui) {
    updateGUIChangeMode(inst, ui);
    onChangeSetCustomProfile(inst, ui);
}

function onChangeBasicBitRateCalc(inst, ui)
{
    updateGUIbasicBitRateCalc(inst, ui);
    onChangeSetCustomProfile(inst, ui);
}

function onChangeFrameFormat(inst, ui)
{
    onFrameFormatEnable(inst,ui);
    onChangeSetCustomProfile(inst, ui);
}

function onChangeParity(inst, ui)
{
    onParityEnable(inst,ui);
    onChangeSetCustomProfile(inst, ui);
}

function onChangeDefaultChipSelect(inst, ui)
{
    /*
     * Dynamically reassign Default Chip Select to be the first item in the
     * chipSelect array. If the array is empty, set Default Chip Select to
     * Chip Select 0
     */
    let chipSelectArg = (inst.chipSelect.length) ? inst.chipSelect[0] : "0";

    let updatedField = {
        defaultChipSelect: chipSelectArg,
    }

    Object.assign(inst, updatedField);
}

/* QSPI does not support CD Mode - removed onChangeCDMode and onChangeCDModeValue functions */

/************************* General functions **********************************/
/*
 *  ======== getQSPIClockSourceFreq ========
 *  Get the QSPI Clock source frequency in Hz
 *
 */
function getQSPIClockSourceFreq(inst) {
    let qspiClockFreq = 0;
    /* QSPI only supports BUSCLK */
   switch (inst.qspiClkSrc){
       case "BUSCLK":
           qspiClockFreq = Common.getBUSCLKFreq(inst, "QSPI");
        break;
   }
   qspiClockFreq = qspiClockFreq / inst.qspiClkDiv;

   return qspiClockFreq;
}

/*
 *  ======== getValueBitRateCalc ========
 *  Calculate the actual QSPI bit rate based on clock source
 *  This is used for displaying to inst.calculatedBitRate
 */
function getValueBitRateCalc(inst) {
    let clockFreq;
    let bitRate;
    let scr;
    let calcBitRate;
    let bitArray = [];

    bitRate = inst.targetBitRate;
    clockFreq = getQSPIClockSourceFreq(inst);

    scr = (Math.round(clockFreq / (2*bitRate)) - 1);

    /* Clamp SCR to valid range 0-1023 */
    if (scr < 0) scr = 0;
    if (scr > 1023) scr = 1023;

    calcBitRate = clockFreq / ((1 + scr)*2);
    bitArray.push({name: 0, displayName: calcBitRate.toFixed(2)});

    if(scr - 1 >= 0)
    {
        calcBitRate = clockFreq / ((1 + (scr-1))*2);
        bitArray.push({name: -1, displayName: calcBitRate.toFixed(2)});
    }

    if ((scr + 1) <= 1023)
    {
        calcBitRate = clockFreq / ((1 + (scr+1))*2);
        bitArray.push({name: 1, displayName: calcBitRate.toFixed(2)});
    }

    return bitArray;
}

/*
 *  ======== getSelectedBitRateCalc ========
 * Calculate the QSPI bit rate values using the offset from the option that the
 * user selects in inst.calculatedBitRate.
 * This is used for the error calculation in inst.calculatedError
 *
 */
function getSelectedBitRateCalc(inst) {
    let clockFreq;
    let bitRate;
    let scr;
    let calcBitRate;
    let scrOffset;

    bitRate = inst.targetBitRate;
    clockFreq = getQSPIClockSourceFreq(inst);

    scr = (Math.round(clockFreq / (2*bitRate)) - 1);
    scrOffset = inst.calculatedBitRate; /* This gets the offset */
    scr = scr + scrOffset;

    /* Clamp SCR to valid range 0-1023 */
    if (scr < 0) scr = 0;
    if (scr > 1023) scr = 1023;

    calcBitRate = clockFreq / ((1 + scr)*2);

    return calcBitRate;
}

function getSCR(inst) {
    let clockFreq;
    let bitRate;
    let scr;
    let scrOffset;

    bitRate = inst.targetBitRate;
    clockFreq = getQSPIClockSourceFreq(inst);

    scr = (Math.round(clockFreq / (2*bitRate)) - 1);
    scrOffset = inst.calculatedBitRate; /* This gets the offset */
    scr = scr + scrOffset;

    /* Clamp SCR to valid range 0-1023 */
    if (scr < 0) scr = 0;
    if (scr > 1023) scr = 1023;

    return scr;
}

/*
 *  ======== getErrorCalc ========
 *  Calculate the QSPI baud error percentage
 *
 */
function getErrorCalc(inst) {
    let error;
    let calcBItRate = getSelectedBitRateCalc(inst);

    error = ((calcBItRate - inst.targetBitRate) / inst.targetBitRate * 100).toFixed(4);

    return parseFloat(Math.abs(error));
}

function getDelayedSamplingCalc(inst) {
    let sourceClkFreq;
    let delay;

    sourceClkFreq = getQSPIClockSourceFreq(inst);

    delay = (inst.delayedSampling) / sourceClkFreq;

    return delay;
}

/*
 *  ======== validatePinmux ========
 *  Validation run after solution
 *
 *  param inst       - module instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validatePinmux(inst, validation) {
    /* QSPI is in PD1 domain - registers are not retained in low power mode */
    /* No retention validation needed for QSPI */
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  param inst       - module instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);

    /* Check clocks are initialized properly */
    if(inst.basicClockSourceCalculated == 0)
    {
        let str = "Clock Source " + inst.qspiClkSrc + " not properly initialized in SYSCTL";
        validation.logError(str, inst, "qspiClkSrc");
    }

    /* QSPI supports up to 40MHz SCLK */
    let maxFrequency = 40000000;

    /* Check frame size is valid */
    if ((inst.dataSize < 4) || (inst.dataSize > 16)) {
        let str = "The frame size must be between 4-16 bits for Controller mode";
        validation.logError(str, inst, "dataSize");
    }

    /* Check that SCLK rate is valid */
    if((inst.targetBitRate) > maxFrequency) {
        let str = "QSPI supports bit rates up to 40MHz. The input clock frequency must be at least 2x faster than the QSPI output clock.";
        validation.logError(str, inst, "targetBitRate");
        validation.logError(str, inst, "basicClockSourceCalculated");
    }

    /* Check that SCR is within valid range (0-1023) */
    let clockFreq = getQSPIClockSourceFreq(inst);
    let rawSCR = (Math.round(clockFreq / (2 * inst.targetBitRate)) - 1) + inst.calculatedBitRate;
    if (rawSCR < 0) {
        let str = "The target bit rate is too high for the current clock source. The calculated SCR (" + rawSCR + ") must be >= 0.";
        validation.logError(str, inst, "targetBitRate");
    }
    if (rawSCR > 1023) {
        let str = "The target bit rate is too low for the current clock source. The calculated SCR (" + rawSCR + ") exceeds the maximum value of 1023.";
        validation.logError(str, inst, "targetBitRate");
    }

    /* QSPI only supports Controller mode - no Peripheral mode validation needed */
    /* QSPI does not support CD Mode - no CD mode validation needed */

    /* Check that at least one CS pin is selected in 4-pin modes */
    if (inst.frameFormat != "MOTO3")
    {
        if (inst.chipSelect.length == 0)
        {
            let str = "A 4-pin frame format has been selected, so you must select at least one Chip Select.";
            validation.logError(str, inst, "chipSelect");
        }
    }
}

/* PROFILES CONFIGURATION */
let config = [
    /* Show selected peripheral below instance name */
    {
        name: "selectedInstance",
        displayName: "Selected Peripheral",
        default: "",
        longDescription: `Refer to the PinMux section for peripheral selection. Chosen instance name is not indicative of active peripheral.`,
        getValue: (inst) => {
            let solution = "";
            try{
                solution = inst.peripheral.$solution.peripheralName;
            }catch (e) {
                // do nothing
            }
            return solution;
        }
    },
    /****** PROFILES CONFIGURATION *******/
    {
        name: "GROUP_PROFILE",
        displayName: "Quick Profiles",
        description: "",
        collapsed: false,
        config: [
            {
                name        : "profile",
                displayName : "QSPI Profiles",
                description : 'Pre-defined profiles for typical QSPI configurations',
                longDescription: `
The Quad Serial Peripheral Interface (QSPI) module provides a high-speed serial interface to transfer data between
MSPM33 devices and external QSPI flash or other QSPI-compatible devices.

The Quick Profile Options are:
* **Standard Dual Mode (PICO/POCI) at 32 MHz**:
    * Controller mode with PICO and POCI data lines
    * 32 MHz bit rate using BUSCLK
    * Motorola 4-wire frame format
    * Polarity Low, Phase First Edge (0, 0)
    * Frame size is 8 bits
    * CS0 selected
    * No interrupts enabled by default
* **Quad Mode (PICO/POCI/IO2/IO3) at 32 MHz**:
    * Controller mode with all four data lines (PICO, POCI, IO2, IO3)
    * 32 MHz bit rate using BUSCLK
    * Motorola 4-wire frame format
    * Polarity Low, Phase First Edge (0, 0)
    * Frame size is 8 bits
    * CS0 selected
    * No interrupts enabled by default
* **Custom**:
    * Allows custom configuration.

Note: QSPI supports up to 40 MHz SCLK. Only Controller mode, BUSCLK clock source, MSB bit order are supported. Packing is disabled.`,
                hidden      : false,
                default     : "CUSTOM",
                options     : [
                    {name: "CONFIG_PROFILE_1", displayName: "Standard Dual Mode (PICO/POCI) at 32 MHz"},
                    {name: "CONFIG_PROFILE_2", displayName: "Quad Mode (PICO/POCI/IO2/IO3) at 32 MHz"},
                    {name: "CUSTOM", displayName: "Custom"},
                ],
                onChange    : onChangeQSPIProfile,
            },
        ],
    },
]

config = config.concat([
    {
        name: "GROUP_BASIC",
        displayName: "Basic Configuration",
        description: "",
        longDescription:
`Basic QSPI options allow configuration of the most common options to enable
basic communication and configure the serial interface.
`,
        collapsed: false,
        config: [
        {
            name: "GROUP_INIT",
            displayName: "QSPI Initialization Configuration",
            description: "",
            longDescription: "Initializes QSPI for selected operation",
            collapsed: false,
            config: [
                    {
                        name        : "mode",
                        displayName : "Mode Select",
                        description : 'QSPI only supports Controller Mode',
                        hidden      : false,
                        readOnly    : true,
                        default     : "CONTROLLER",
                    },
                    {
                        name: "GROUP_BASIC_CLOCK_CONFIG",
                        displayName: "Clock Configuration",
                        description: "",
                        collapsed: true,
                        config: [
                        {
                            name        : "qspiClkSrc",
                            displayName : "Clock Source",
                            description : 'QSPI only supports BUSCLK as clock source',
                            longDescription:`
    QSPI only supports BUSCLK as the clock source.\n
    BUSCLK depends on the power domain:
        * If the QSPI is in PD1, BUSCLK is derived from MCLK, or,
        * If the QSPI is in PD0, BUSCLK is derived from ULPCLK.\n
    The frequency of the clock sources are configured
    in the __SYSCTL Module__. `,
                            readOnly    : true,
                            default     : "BUSCLK",
                        },
                        {
                            name        : "qspiClkDiv",
                            displayName : "Clock Divider",
                            description : 'Clock divider selection',
                            default     : "1",
                            options     : [
                                {name: "1", displayName: "Divided by 1"},
                                {name: "2", displayName: "Divided by 2"},
                                {name: "3", displayName: "Divided by 3"},
                                {name: "4", displayName: "Divided by 4"},
                                {name: "5", displayName: "Divided by 5"},
                                {name: "6", displayName: "Divided by 6"},
                                {name: "7", displayName: "Divided by 7"},
                                {name: "8", displayName: "Divided by 8"},
                            ],
                            onChange    : onChangeSetCustomProfile,
                        },
                        {
                            name        : "basicClockSourceCalculated",
                            displayName : "Calculated Clock Source",
                            description : 'Calculated QSPI Clock Source in Hz',
                            hidden      : false,
                            getValue    : (inst) => getQSPIClockSourceFreq(inst),
                            default     : 0
                        },
                    ]
                },
                    {
                        name        : "targetBitRate",
                        displayName : "Target Bit Rate (Hz)",
                        description : 'The target bit rate in Hz for the serial output clock',
                        longDescription:`
QSPI includes a programmable bit rate clock divider and prescaler to generate the serial output clock.\n
QSPI supports bit rates up to 40MHz SCLK.\n
The input clock frequency must be at least 2x faster than the QSPI output clock.`,
                        hidden      : false,
                        default     : 8000000,
                        range       : [0, 40000000],
                        onChange    : onChangeBasicBitRateCalc
                    },
                    {
                        name        : "calculatedBitRate",
                        displayName : "Calculated Bit Rate",
                        description : 'The calculated bit rate in Hz',
                        longDescription:`
This setting shows the closest calculated bit rates based on the target bit rate.\n
The next available options are shown in drop-down for convenience purposes
and can be selected if needed.\n
The serial bit rate is derived by dividing down the input clock. \n
First, the clock is divided by a ratio selected by the clock divider. The clock
is further divided by a value from 1 to 1024, which is 1 + SCR, where SCR is the
serial clock divider ratio value programmed in the CLKCTL.SCR register.\n
The equation below defines the frequency of the output clock:\n
        bitRate = (qspiClock) / ((1 + SCR) * 2)\n
The factor of two (*2) is set by CLKDIV and the input clock must be at least
2 times faster than QSPI output clock.\n
Given the QSPI clock and the Target Bit Rate, the SCR can be calculated with the
following equation:\n
        SCR = (qspiClock) / (2 * bitRate)) - 1)
`,
                        hidden      : false,
                        options    : (inst) => getValueBitRateCalc(inst),
                        default     : 0
                    },
                    {
                        name        : "scr", /* Used internally for code gen */
                        displayName : "SCR",
                        description : 'The scr',
                        hidden      : true,
                        getValue    : (inst) => getSCR(inst),
                        default     : 0
                    },
                    {
                        name        : "calculatedBitRateNum", /* Used internally for code gen */
                        displayName : "Calculated Bit Rate",
                        description : 'The calculated bit rate in Hz',
                        hidden      : true,
                        getValue    : (inst) => parseFloat(getSelectedBitRateCalc(inst).toFixed(2)),
                        default     : 0
                    },
                    {
                        name        : "calculatedError",
                        displayName : "Calculated Error (%)",
                        description : 'The calculated error percentage',
                        longDescription: `
This shows the calculated baud rate error percentage.\n
This is calculated by:\n
        error = (calculatedBitRate - targetBitRate) / (targetBitRate * 100)\n
`,
                        hidden      : false,
                        getValue    : (inst) => getErrorCalc(inst),
                        default     : 0
                    },
                    /* DL_QSPI_setFrameFormat */
                    {
                        name: "frameFormat",
                        displayName: "Frame Format",
                        description: "The protocol format for the QSPI mode",
                        longDescription: `
The supported options are: Motorola 4-wire, Motorola 3-wire, and Texas Instruments
Synchronous.\n
__Motorola SPI Frame Format:__\n
The Motorola SPI interface is a 4-wire interface where the CS signal behaves
as a peripheral select. In the 3-wire mode the CS signals is not required and
the module behaves as if always selected. The main feature of the Motorola SPI
format is that the inactive state and phase of the SCLK signal can be programmed
through the clock polarity (SPO) and phase (SPH) bits.\n

* Signal sequences for Motorola SPI format with SPO = 0 and SPH = 0.\n
    ![SPI Motorola SPO0 SPH0](../docs/english/sysconfig/images/spimotospo0sph0M0G3xx.png "SPI Motorola Diagram SPO0 SPH0")\n

* Signal sequences for Motorola SPI format with SPO = 0 and SPH = 1.\n
    ![SPI Motorola SPO0 SPH1](../docs/english/sysconfig/images/spimotospo0sph1M0G3xx.png "SPI Motorola Diagram SPO0 SPH1")\n

* Signal sequences for Motorola SPI format with SPO = 1 and SPH = 0.\n
    ![SPI Motorola SPO1 SPH0](../docs/english/sysconfig/images/spimotospo1sph0M0G3xx.png "SPI Motorola Diagram SPO1 SPH0")\n

* Signal sequences for Motorola SPI format with SPO = 1 and SPH = 1.\n
    ![SPI Motorola SPO1 SPH1](../docs/english/sysconfig/images/spimotospo1sph1M0G3xx.png "SPI Motorola Diagram SPO1 SPH1")\n

 __Texas Instruments Synchronous:__\n
SCLK and CS are forced low and the transmit data line PICO is put in tristate whenever the SPI is idle. When
the bottom entry of the TX FIFO contains data, CS is pulsed high for one SCLK period. The transmitted value
is also transferred from the TX FIFO to the serial shift register of the transmit logic. On the next rising edge of
SCLK, the MSB of the 4- to 16-bit data frame is shifted out on the PICO pin. Likewise, the MSB of the received
data is shifted onto the POCI pin by the off-chip serial peripheral device. Both the SPI and the off-chip serial
peripheral device then clock each data bit into their serial shifter on each falling edge of SCLK. The received
data is transferred from the serial shifter to the RX FIFO on the first rising edge of SCLK after the least significant
bit (LSB) is latched.\n
The serial clock (SCLK) is held inactive while the SPI is idle and SCLK transitions at the programmed frequency
only during active transmission or reception of data. The IDLE state of SCLK provides a receive time-out
indication that occurs when the RX FIFO still contains data after a time-out period.\n
![SPI TI Sync](../docs/english/sysconfig/images/spitisyncM0G3xx.png "SPI TI Sync Diagram")\n
`,
                        hidden      : false,
                        default: "MOTO4",
                        options: [
                            { name: "MOTO4", displayName: "Motorola 4-wire" },
                            { name: "MOTO3", displayName: "Motorola 3-wire" },
                            { name: "TI_SYNC", displayName: "TI Synchronous" },
                        ],
                        onChange :  onChangeFrameFormat,
                    },
                    {
                        name: "polarityPhase",
                        displayName: "Clock Polarity and Phase",
                        description: "The clock polarity and phase combination",
                        longDescription: `
QSPI only supports specific combinations of clock polarity (SPO) and phase (SPH):\n
* (0, 0): Low polarity, data captured on first clock edge\n
* (1, 1): High polarity, data captured on second clock edge\n
These are the only valid combinations for QSPI operation.
`,
                        hidden      : false,
                        default: "0_0",
                        options: [
                            { name: "0_0", displayName: "Polarity Low, Phase First Edge (0, 0)" },
                            { name: "1_1", displayName: "Polarity High, Phase Second Edge (1, 1)" },
                        ],
                        onChange : onChangeSetCustomProfile,
                    },
                    {
                        name: "polarity",
                        displayName: "Clock Polarity",
                        description: "The clock polarity (derived from polarityPhase)",
                        hidden      : true,
                        default: "0",
                        getValue: (inst) => inst.polarityPhase.split("_")[0],
                    },
                    {
                        name: "phase",
                        displayName: "Phase",
                        description: "The clock phase (derived from polarityPhase)",
                        hidden      : true,
                        default: "0",
                        getValue: (inst) => inst.polarityPhase.split("_")[1],
                    },
                    /* DL_QSPI_setDataSize */
                    {
                        name: "dataSize",
                        displayName: "Frame Size (bits)",
                        description: "The bit length of each frame",
                        longDescription: `
The bit length per transfer is defined between 4-16 bits for Controller mode and 7-16 bits for Peripheral mode.\n
A transfer will be triggered with writing to the TX buffer register. The data write needs to have at least the number
of bits of the transfer. For example, if only a byte is written to the TX buffer but the length of the transfer is > 8,
then the missing bits will be filled with 0s.\n
On the receive path the received data will be moved to the RXFIFO or RX buffer after the number of bit defined
have been received.
`,
                        hidden      : false,
                        default: 8,
                        options: [
                            { name: 4, displayName: "4" },
                            { name: 5, displayName: "5" },
                            { name: 6, displayName: "6" },
                            { name: 7, displayName: "7" },
                            { name: 8, displayName: "8" },
                            { name: 9, displayName: "9" },
                            { name: 10, displayName: "10" },
                            { name: 11, displayName: "11" },
                            { name: 12, displayName: "12" },
                            { name: 13, displayName: "13" },
                            { name: 14, displayName: "14" },
                            { name: 15, displayName: "15" },
                            { name: 16, displayName: "16" },
                        ]
                    },
                    /* DL_QSPI_setBitOrder */
                    {
                        name: "bitOrder",
                        displayName: "Bit Order",
                        description: "QSPI only supports MSB first bit order",
                        longDescription: `
QSPI only supports MSB (Most Significant Bit) first bit order.\n
If the parity is enabled the parity bits is always received as last bit.
`,
                        hidden      : false,
                        readOnly    : true,
                        default: "MSB",
                    },
                    /****** Start of CONTROLLER BASIC CONFIGURATION *******/
                    {
                        name: "basicControllerConfig",
                        displayName: "QSPI Controller Basic Configuration",
                        collapsed: false,
                        config: [
                            /* DL_QSPI_setChipSelect */
                            {
                                name        : "chipSelect",
                                displayName : "Chip Select",
                                hidden      : false,
                                description : 'Choose which chip select should be used for data transfer',
                                longDescription: `
Select which connected QSPI Peripheral device is addressed by the QSPI Controller using up to 4 Chip Select (CS) signals.
Enabling a pin will initialize it and reserve it for QSPI_CS functionality; however, only one CS can be enabled by the
QSPI Controller at a time. Developers can optionally use regular GPIOs as CS by controlling them manually and using
QSPI 3-wire mode.
`,
                                minSelections: 0,
                                default     : ["0"],
                                options     : QSPICSOptions,
                                getDisabledOptions: getQSPICSDisabledOptions,
                                onChange :  (inst, ui) => {
                                    ui.defaultChipSelect.hidden = !(inst.chipSelect.length > 1);
                                    onChangeDefaultChipSelect(inst, ui);
                                    onChangeSetCustomProfile(inst, ui);
                                },
                            },
                            {
                                name        : "defaultChipSelect",
                                displayName : "Default Chip Select",
                                hidden      : true,
                                description : 'Choose which chip select to initially enable',
                                longDescription: `
Configures the Chip Select signal which will be used by default after initialization. Only one CS can be enabled by the
QSPI Controller at a time. Developers can switch to a different CS signal during runtime
(i.e. using DL_QSPI_setControllerChipSelect) in the application code.
`,
                                default     : "0",
                                options     : QSPICSOptions,
                                getDisabledOptions: getQSPIDefaultCSDisabledOptions,
                                onChange :  onChangeSetCustomProfile,
                            },
                        ]
                    }, /****** End  of CONTROLLER BASIC CONFIGURATION *******/
                    /****** Start of PERIPHERAL BASIC CONFIGURATION *******/
                    /* QSPI only supports Controller mode - Peripheral config hidden */
                    {
                        name: "basicPeripheralConfig",
                        displayName: "QSPI Peripheral Basic Configuration",
                        collapsed: false,
                        config: [
                            /* DL_QSPI_setChipSelect */
                            {
                                name        : "peripheralChipSelect",
                                displayName : "Chip Select",
                                hidden      : true,
                                description : 'Choose which chip select should be used for data transfer',
                                longDescription: `
Select the Chip Select to be used by the QSPI Peripheral after initialization.
Only one CS can be enabled by the QSPI Peripheral at a time.
`,
                                default     : "0",
                                options     : QSPICSOptions,
                                onChange :  onChangeSetCustomProfile,
                            },
                        ]
                    }, /****** End of PERIPHERAL BASIC CONFIGURATION *******/
                ]
            },
        ],

    }
])

/* DL_QSPI_DMA_INTERRUPT_RX */
let dmaEvt1Options = [
    {name: "None", displayName: "None"},
    {name: "DL_QSPI_DMA_INTERRUPT_RX", displayName: "QSPI RX interrupt"},
]
/* DL_QSPI_DMA_INTERRUPT_TX */
let dmaEvt2Options = [
    {name: "None", displayName: "None"},
    {name: "DL_QSPI_DMA_INTERRUPT_TX", displayName: "QSPI TX interrupt"},
]

/* QSPI does not support "almost full" and "almost empty" FIFO threshold options */

config = config.concat([
    {
        name: "GROUP_ADVANCED",
        displayName: "Advanced Configuration",
        description: "",
        longDescription: "",
        collapsed: true,
        config: [
            {
                name: "parity",
                displayName: "Parity",
                description: "The type of parity to enable and use for both receive and transmit directions.",
                longDescription: `
Selected parity configuration will be applied for both receive and transmit directions. To only configure parity
When parity check is enabled, an additional bit as parity bit is received and the parity of the
data is calculated and with the received data evaluated. The last bit will be used
as parity to evaluate the integrity of the previous bits.\n
The parity mode can be selected as even or odd.
When detecting a fault, the interrupt flag PER is set (to mark the data as invalid).
`,
                hidden      : false,
                default: "NONE",
                options: [
                    { name: "NONE", displayName: "Disabled" },
                    { name: "ODD", displayName: "Odd Parity" },
                    { name: "EVEN", displayName: "Even Parity" },
                ],
                onChange : onChangeParity,
            },
            {
                name        : "parityDirection",
                displayName : "Parity Direction",
                description : 'Select whether parity is enabled for Transmit only, Receive only, or Transmit and Receive.',
                hidden      : true,
                default     : "TX_RX",
                options     : [
                    {name: "TX_RX", displayName: "Transmit and Receive"},
                    {name: "TX", displayName: "Transmit only"},
                    {name: "RX", displayName: "Receive only"},
                ],
                onChange : onChangeSetCustomProfile,
            },
            {
                name        : "packingEnable",
                displayName : "Packing Feature",
                description : 'QSPI does not support packing. Packing is always disabled.',
                longDescription: `
QSPI does not support the packing feature. Packing is always disabled and
DL_QSPI_disablePacking() is called during initialization.
`,
                hidden      : false,
                readOnly    : true,
                default     : "Disabled",
            },
            /* DL_QSPI_setRXFIFOThreshold */
            {
                name        : "rxFifoThreshold",
                displayName : "RX FIFO Threshold Level",
                description : 'The RX FIFO threshold level for when the interrupt triggers',
                default     : "DL_QSPI_RX_FIFO_LEVEL_1_2_FULL",
                options     : [
                    {name: "DL_QSPI_RX_FIFO_LEVEL_ONE_FRAME", displayName: "RX FIFO contains >= 1 entry"},
                    {name: "DL_QSPI_RX_FIFO_LEVEL_FULL", displayName: "RX FIFO is full"},
                    {name: "DL_QSPI_RX_FIFO_LEVEL_3_4_FULL", displayName: "RX FIFO >= 3/4 full"},
                    {name: "DL_QSPI_RX_FIFO_LEVEL_1_2_FULL", displayName: "RX FIFO >= 1/2 full"},
                    {name: "DL_QSPI_RX_FIFO_LEVEL_1_4_FULL", displayName: "RX FIFO >= 1/4 full"},
                ],
                onChange : onChangeSetCustomProfile,
            },
            /* DL_QSPI_setTXFIFOThreshold */
            {
                name        : "txFifoThreshold",
                displayName : "TX FIFO Threshold Level",
                description : 'The TX FIFO interrupt threshold level',
                default     : "DL_QSPI_TX_FIFO_LEVEL_1_2_EMPTY",
                options     : [
                    {name: "DL_QSPI_TX_FIFO_LEVEL_3_4_EMPTY", displayName: "TX FIFO <= 3/4 empty"},
                    {name: "DL_QSPI_TX_FIFO_LEVEL_1_2_EMPTY", displayName: "TX FIFO <= 1/2 empty"},
                    {name: "DL_QSPI_TX_FIFO_LEVEL_1_4_EMPTY", displayName: "TX FIFO <= 1/4 empty"},
                    {name: "DL_QSPI_TX_FIFO_LEVEL_EMPTY", displayName: "TX FIFO is empty"},
                    {name: "DL_QSPI_TX_FIFO_LEVEL_ONE_FRAME", displayName: "TX FIFO is not full"},
                ],
                onChange : onChangeSetCustomProfile,
            },
            {
                name        : "direction",
                displayName : "Communication Direction",
                description : 'Select the communication direction for QSPI. This affects which pins are available in the PinMux section.',
                longDescription: `
QSPI supports two communication direction modes:\n
* **PICO and POCI**: Standard SPI mode with PICO and POCI pins for bidirectional communication.\n
* **PICO, POCI, IO2, and IO3**: Quad SPI mode with four data lines for higher throughput.
`,
                default     : "PICO_POCI",
                options     : [
                    {name: "PICO_POCI", displayName: "PICO and POCI"},
                    {name: "PICO_POCI_IO2_IO3", displayName: "PICO, POCI, IO2 and IO3"},
                ],
                onChange : onChangeSetCustomProfile,
            },
            /****** Start of CONTROLLER ADVANCED CONFIGURATION *******/
            {
                name: "advancedControllerConfig",
                displayName: "QSPI Controller Advanced Configuration",
                longDescription: "",
                collapsed: false,
                config: [
                    /* QSPI does not support Command Data Mode */
                    /* DL_QSPI_enableLoopbackMode */
                    {
                        name        : "enableInternalLoopback",
                        displayName : "Enable Internal Loopback",
                        description : 'Enables internal loopback mode',
                        longDescription: `
        In loopback mode, data transmitted on the TXD output is received on the RXD input.\n
        Data received on the RXD IO pin will be ignored when loopback is enabled.\n
        `,
                        hidden      : false,
                        default     : false,
                        onChange : onChangeSetCustomProfile,
                    },
                    /* DL_QSPI_setDelayedSampling */
                    {
                        name        : "delayedSampling",
                        displayName : "Delayed Data Sampling Counts",
                        description : 'Set the count in clock steps for delayed data sampling',
                        longDescription: `
When due to run time conditions the input data arrives with some additional delay at the POCI pin and the
following input data sampling stage, the previous data would be sampled at the sampling clock edge. To
compensate that a delayed sampling can be set. The delay is adjusted in steps of QSPI input clock steps.\n
The maximum allowed delay should not exceed the length of one data frame.
`,
                        hidden      : false,
                        default     : 0,
                        range : [0,15],
                        isInteger : true,
                        onChange : onChangeSetCustomProfile,
                    },
                    {
                        name        : "delayedSamplingCalc",
                        displayName : "Calculated Delayed Sampling",
                        description : 'Calculated time for delayed data sampling',
                        hidden      : false,
                        default     : "0s",
                        getValue    : (inst) => (Common.getUnitPrefix((getDelayedSamplingCalc(inst))).str+'s'),
                    },
                    /* DL_QSPI_setChipSelectAssertDelay */
                    {
                        name        : "chipSelectAssertDelay",
                        displayName : "Chip Select Assert Delay",
                        description : 'Set the delay before chip select is asserted',
                        longDescription: `
Configures the delay in SCLK cycles before the chip select signal is asserted.\n
Options:\n
* **0.5 SCLK Delay**: Half SCLK cycle delay (default)\n
* **1.5 SCLK Delay**: One and a half SCLK cycle delay
`,
                        hidden      : false,
                        default     : "DL_QSPI_CHIP_SELECT_ASSERT_DELAY_0P5",
                        options     : [
                            {name: "DL_QSPI_CHIP_SELECT_ASSERT_DELAY_0P5", displayName: "0.5 SCLK Delay"},
                            {name: "DL_QSPI_CHIP_SELECT_ASSERT_DELAY_1P5", displayName: "1.5 SCLK Delay"},
                        ],
                        onChange : onChangeSetCustomProfile,
                    },
                    /* DL_QSPI_setChipSelectDeAssertDelay */
                    {
                        name        : "chipSelectDeAssertDelay",
                        displayName : "Chip Select De-Assert Delay",
                        description : 'Set the delay before chip select is de-asserted',
                        longDescription: `
Configures the delay in SCLK cycles before the chip select signal is de-asserted.\n
Options:\n
* **0.5 SCLK Delay**: Half SCLK cycle delay (default)\n
* **1.5 SCLK Delay**: One and a half SCLK cycle delay
`,
                        hidden      : false,
                        default     : "DL_QSPI_CHIP_SELECT_DEASSERT_DELAY_0P5",
                        options     : [
                            {name: "DL_QSPI_CHIP_SELECT_DEASSERT_DELAY_0P5", displayName: "0.5 SCLK Delay"},
                            {name: "DL_QSPI_CHIP_SELECT_DEASSERT_DELAY_1P5", displayName: "1.5 SCLK Delay"},
                        ],
                        onChange : onChangeSetCustomProfile,
                    },
                    /* DL_QSPI_setDataSampleDelay */
                    {
                        name        : "dataSampleDelay",
                        displayName : "Delay in Internal SCLK Sampling",
                        description : 'Enable delayed clock sampling using internal SCLK loopback',
                        longDescription: `
Similar to delayed data sampling, QSPI has an option to sample the data on a delayed clock. In this case, the
SCLK is looped-back internally through an IO cell of the SOC and fed back to the QSPI. The delay in this case is
fixed. The clock loopback can be enabled by setting the QSPITIMING.DATSMPLDY bit.\n
Options:\n
* **No Delay**: No additional delay in SCLK sampling (default)\n
* **1 Clock Delay**: 1 clock cycle delay in SCLK sampling
`,
                        hidden      : false,
                        default     : "DL_QSPI_DATA_SAMPLE_DELAY_0",
                        options     : [
                            {name: "DL_QSPI_DATA_SAMPLE_DELAY_0", displayName: "No Delay"},
                            {name: "DL_QSPI_DATA_SAMPLE_DELAY_1", displayName: "1 Clock Delay"},
                        ],
                        onChange : onChangeSetCustomProfile,
                    },
                ]
            }, /****** End  of CONTROLLER ADVANCED CONFIGURATION *******/
            /****** Start of PERIPHERAL ADVANCED CONFIGURATION *******/
            {
                name: "advancedPeripheralConfig",
                displayName: "QSPI Peripheral Advanced Configuration",
                longDescription: "",
                collapsed: false,
                config: [
                    /* DL_QSPI_enablePeripheralAlignDataOnChipSelect */
                    {
                        name        : "clearRXCounterOnCSIdle",
                        displayName : "Enable Clear Shift Ctr On CS Idle",
                        hidden      : true,
                        description : 'Enable clearing the shift counter when the CS signal is idle',
                        longDescription: `
When enabled, the shift counter on CS idle is cleared automatically when CS gets set
inactive. This helps the peripheral to synchronize again on the controller in case of a disturbance or glitch on the
clock line or during initialization.\n
This is relevant only in the peripheral mode.`,
                        default     : false,
                        onChange : onChangeSetCustomProfile,
                    },
                ]
            }, /****** End  of PERIPHERAL ADVANCED CONFIGURATION *******/
            {
                name: "GROUP_RETENTION",
                displayName: "Retention Configuration",
                description: "",
                longDescription: `QSPI is in PD1 domain. Registers are not retained in low power mode.`,
                collapsed: false,
                config: [
                    {
                        name        : "retentionStatus",
                        displayName : "Retention Status",
                        description : 'QSPI registers are not retained in low power mode (PD1 domain)',
                        longDescription: `
QSPI is located in the PD1 power domain. In low power modes, the PD1 domain is powered down
and therefore QSPI registers are not retained. The peripheral must be re-initialized after
exiting low power mode.
`,
                        hidden      : false,
                        readOnly    : true,
                        default     : "Registers Not Retained",
                    },
                ],
            },
        ]
    },
    {
        name: "GROUP_INTERRUPTS",
        displayName: "Interrupt Configuration",
        longDescription: "This section allows enabling/disabling QSPI interrupts",
        collapsed: true,
        config: [
            /* DL_QSPI_enableInterrupt */
            {
                name        : "enabledInterrupts",
                displayName : "Enable Interrupts",
                description : 'Enable QSPI interrupt',
                default     : [],
                options     : getIntOptions,
                onChange    : (inst, ui) => {
                    updateGUIInterrupts(inst, ui);
                    onChangeSetCustomProfile(inst, ui);
                }
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
]);

config = config.concat([
    {
        name: "GROUP_DMA",
        displayName: "DMA Configuration",
        longDescription: `
This section allows configuring DMA Event triggers.\n
The QSPI module can set trigger conditions for the DMA through receive or
transmit events. \n
When the DMA functionality is enabled, the QSPI asserts a DMA request
when the configured receive or transmit trigger condition occurs. \n
`,
        collapsed: true,
        config: [
            /* DL_QSPI_enableDMAReceiveEvent */
            {
                name        : "enabledDMAEvent1Triggers",
                displayName : "Configure DMA Event 1 Triggers",
                description : 'Configure the QSPI interrupts to use as triggers for the DMA RX event',
                hidden      : false,
                default     : "None",
                options     : dmaEvt1Options,
                onChange    : (inst,ui) => {
                    ui.enableDMAEvent1.hidden = (inst.enabledDMAEvent1Triggers == "None");
                    onChangeSetCustomProfile(inst,ui);
                },
            },
            {
                name        : "enableDMAEvent1",
                displayName : "Enable DMA Event 1 Trigger",
                longDescription: `Only one DMA receive trigger can be enabled
                at the same time. If left unchecked, the module-specific enable
                DMA API should be called in the application code.`,
                hidden      : true,
                default     : true,
                onChange    : onChangeSetCustomProfile,
            },
            /* DL_QSPI_enableDMATransmitEvent */
            {
                name        : "enabledDMAEvent2Triggers",
                displayName : "Configure DMA Event 2 Trigger",
                description : 'Configure the QSPI interrupt to use as triggers for the DMA TX event',
                hidden      : false,
                default     : "None",
                options     : dmaEvt2Options,
                onChange    : (inst,ui) => {
                    ui.enableDMAEvent2.hidden = (inst.enabledDMAEvent1Triggers == "None");
                    onChangeSetCustomProfile(inst,ui);
                },
            },
            {
                name        : "enableDMAEvent2",
                displayName : "Enable DMA Event 2 Trigger",
                longDescription: `If left unchecked, the module-specific enable
                DMA API should be called in the application code.`,
                hidden      : true,
                default     : true,
                onChange    : onChangeSetCustomProfile,
            },
        ]
    },
]);

/* Add Pinmux Peripheral Configuration group */
config = config.concat(Common.getGPIOGroupConfig());

function moduleInstances(inst){
    let modInstances = []

    let rxPassedTriggerSelect = 0;
    let txPassedTriggerSelect = 1;
    /* DMA Channel configuration */
    if(!["None"].includes(inst.enabledDMAEvent1Triggers)){
        let mod = {
            name: "DMA_CHANNEL_EVENT1",
            displayName: "DMA Channel Event 1",
            moduleName: '/ti/driverlib/DMAChannel',
            group: "GROUP_DMA",
            args: {
            },
            requiredArgs: {
                hideTriggerSelect: true,
                passedTriggerSelect: rxPassedTriggerSelect,
            },
        }
        modInstances.push(mod);
    }
    if(!["None"].includes(inst.enabledDMAEvent2Triggers)){
        let mod = {
            name: "DMA_CHANNEL_EVENT2",
            displayName: "DMA Channel Event 2",
            moduleName: '/ti/driverlib/DMAChannel',
            group: "GROUP_DMA",
            args: {
            },
            requiredArgs: {
                hideTriggerSelect: true,
                passedTriggerSelect: txPassedTriggerSelect,
            },
        }
        modInstances.push(mod);
    }

    /*
     * Gets a Peripheral GPIO Configuration submodule
     */
    /* CONDITIONS CODE START */
    let sclkConfig, picoConfig, pociConfig, io2Config, io3Config, cs0Config, cs1Config, cs2Config, cs3Config = false;

    sclkConfig = true;

    /* QSPI direction options */
    switch(inst.direction) {
        case 'PICO_POCI':
            picoConfig = true;
            pociConfig = true;
            io2Config = false;
            io3Config = false;
            break;
        case 'PICO_POCI_IO2_IO3':
            picoConfig = true;
            pociConfig = true;
            io2Config = true;
            io3Config = true;
            break;
    }

    /* QSPI only supports Controller mode */
    if (inst.frameFormat == "MOTO4" || inst.frameFormat == "TI_SYNC") {
        if (inst.chipSelect.includes("0"))
        {
            cs0Config = true;
        }
        if (inst.chipSelect.includes("1"))
        {
            cs1Config = true;
        }
        if (inst.chipSelect.includes("2"))
        {
            cs2Config = true;
        }
        if (inst.chipSelect.includes("3"))
        {
            cs3Config = true;
        }
    }
    /* CONDITIONS CODE END */

    /* SCLK */
    /* SCLK pin does not support output inversion */
    Common.pushGPIOConfigInstHideOutInv(inst, modInstances,   sclkConfig,    "sclkPin",
     "SCLK", "QSPI SCLK (Clock)", "OUTPUT");
    /* PICO */
    Common.pushGPIOConfigInst(inst, modInstances,   picoConfig,    "picoPin",
     "PICO", "QSPI PICO", "OUTPUT");
    /* POCI */
    /* POCI pin does not support output inversion */
    Common.pushGPIOConfigInstHideOutInv(inst, modInstances,   pociConfig,    "pociPin",
     "POCI", "QSPI POCI", "INPUT");
    /* IO2 */
    Common.pushGPIOConfigInst(inst, modInstances,   io2Config,    "io2Pin",
     "IO2", "QSPI IO2", "OUTPUT");
    /* IO3 */
    Common.pushGPIOConfigInst(inst, modInstances,   io3Config,    "io3Pin",
     "IO3", "QSPI IO3", "OUTPUT");
    /* CS0 */
    Common.pushGPIOConfigInst(inst, modInstances,   cs0Config,    "cs0Pin",
     "CS0", "QSPI CS0 (Chip Select 0)", "OUTPUT");
    /* CS1 */
    Common.pushGPIOConfigInst(inst, modInstances,   cs1Config,     "cs1Pin",
     "CS1", "QSPI CS1 (Chip Select 1)", "OUTPUT");
    /* CS2 */
    Common.pushGPIOConfigInst(inst, modInstances,   cs2Config,     "cs2Pin",
     "CS2", "QSPI CS2 (Chip Select 2)", "OUTPUT");
    /* CS3 */
    Common.pushGPIOConfigInst(inst, modInstances,   cs3Config,     "cs3Pin",
     "CS3", "QSPI CS3 (Chip Select 3)", "OUTPUT");

    return modInstances;

}

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base module configuration
 */
let devSpecific = {

    config: config,


    maxInstances: 1,  /* QSPI only supports one instance */

    /* override generic requirements with  device-specific reqs (if any) */
    validate: validate,

    validatePinmux: validatePinmux,

    pinmuxRequirements: pinmuxRequirements,

    moduleInstances: moduleInstances,

    filterHardware: filterHardware,

    /* override device-specific templates */
    templates: {
        boardc: "/ti/driverlib/qspi/QSPI.Board.c.xdt",
        boardh: "/ti/driverlib/qspi/QSPI.Board.h.xdt",
        Call: true,
        Reset: true,
        Power: true,
        GPIO: true,
        Function: true,
        Retention: false,  /* QSPI is in PD1 domain - registers not retained in low power mode */

    },
    modules: setRequiredModules,


    _getPinResources: _getPinResources
};

function setRequiredModules(inst){
    let theModules = ["Board", "SYSCTL"]

    if(!["None"].includes(inst.enabledDMAEvent1Triggers) || !["None"].includes(inst.enabledDMAEvent2Triggers)){
        theModules.push("DMA");
    }

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
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
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

// QSPI interface pin names from device JSON
let cs1interfaces = ["CS1_POCI1"];
let cs2interfaces = ["CS2_POCI2"];
let cs3interfaces = ["CS3_CD_POCI3"];

/*  ======== pinmuxRequirements ========
 *  Returns peripheral pin requirements of the specified instance
 *
 *  param inst    - a fully configured module instance
 *
 *  returns req[] - an array of pin requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let sclk = {
        name              : "sclkPin",  /* config script name */
        displayName       : "QSPI SCLK (Clock)", /* GUI name */
        interfaceNames    : ["SCLK"]   /* pinmux tool name */
    };

    let pico = {
        name              : "picoPin",
        displayName       : "QSPI PICO",
        interfaceNames    : ["PICO"]
    };
    let poci = {
        name              : "pociPin",
        displayName       : "QSPI POCI",
        interfaceNames    : ["POCI"]
    };
    let io2 = {
        name              : "io2Pin",
        displayName       : "QSPI IO2",
        interfaceNames    : ["IO2"]
    };
    let io3 = {
        name              : "io3Pin",
        displayName       : "QSPI IO3",
        interfaceNames    : ["IO3"]
    };

    let cs0 = {
        name              : "cs0Pin",
        displayName       : "QSPI CS0 (Chip Select 0)",
        interfaceNames    : ["CS0"]
    };

    let cs1 = {
        name              : "cs1Pin",
        displayName       : "QSPI CS1 (Chip Select 1)",
        interfaceNames    : cs1interfaces
    };

    let cs2 = {
        name              : "cs2Pin",
        displayName       : "QSPI CS2 (Chip Select 2)",
        interfaceNames    : cs2interfaces
    };

    let cs3 = {
        name              : "cs3Pin",
        displayName       : "QSPI CS3 (Chip Select 3)",
        interfaceNames    : cs3interfaces
    };

    let resources = [];

    resources.push(sclk);

    /* QSPI direction options */
    switch(inst.direction) {
        case 'PICO_POCI':
            resources.push(pico);
            resources.push(poci);
            break;
        case 'PICO_POCI_IO2_IO3':
            resources.push(pico);
            resources.push(poci);
            resources.push(io2);
            resources.push(io3);
            break;
    }

    /* QSPI only supports Controller mode */
    if (inst.frameFormat == "MOTO4" || inst.frameFormat == "TI_SYNC") {
        if (inst.chipSelect.includes("0"))
        {
            resources.push(cs0);
        }
        if (inst.chipSelect.includes("1"))
        {
            resources.push(cs1);
        }
        if (inst.chipSelect.includes("2"))
        {
            resources.push(cs2);
        }
        if (inst.chipSelect.includes("3"))
        {
            resources.push(cs3);
        }
    }

    /* Build signalTypes dynamically based on direction */
    let signalTypesObj = {
        sclkPin     : ['QSPI_SCLK'],
        picoPin     : ['QSPI_PICO'],
        pociPin     : ['QSPI_POCI'],
    };

    /* Add IO2/IO3 signal types only when quad mode is selected */
    if (inst.direction == 'PICO_POCI_IO2_IO3') {
        signalTypesObj.io2Pin = ['QSPI_IO2'];
        signalTypesObj.io3Pin = ['QSPI_IO3'];
    }

    /* Add CS signal types */
    signalTypesObj.cs0Pin = ['QSPI_CS0'];
    signalTypesObj.cs1Pin = ['QSPI_CS1'];
    signalTypesObj.cs2Pin = ['QSPI_CS2'];
    signalTypesObj.cs3Pin = ['QSPI_CS3'];

    let qspi = {
        name          : "peripheral",
        displayName   : "QSPI Peripheral",
        interfaceName : "QSPI",
        resources     : resources,
        signalTypes   : signalTypesObj
    };
    return [qspi];
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
    let modulePin;

    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["QSPI"])) modulePin = sig;
    }
    let result = modulePin ? true : false;

    return (result);
}

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
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(devSpecific.config);

    return (result);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base module module */
    extend: extend
};
