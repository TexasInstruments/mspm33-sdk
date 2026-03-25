/*
 * Copyright (c) 2025, Texas Instruments Incorporated
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
 */

 #include "ADXL355.h"

 /* ARM CMSIS-DSP Header files */
 #include "stdlib.h"
 #include "string.h"

 /* DAP includes */
 #include <ti/edgeAI/m33/device_agent_protocol/Dap.h>
#include <ti/hal/mspm33/include/Uart_Hal.h>

 /** \brief Size of the class buffer */
 #define SAMPLE_BUFFER_SIZE (10)

 /** \brief Number of input channels */
 #define INPUT_CHANNELS     (3)
 #define BUFFER_LEN          32

 /** \brief Data points of a single frame */
 volatile int8_t input_data[BUFFER_LEN*INPUT_CHANNELS];

 /** \brief Sensor configuration structure */
 volatile sensorStruct sensor;

 /** \brief DAP specific variables */

 /* Sensor JSON strings */
 volatile static const char Dap_Sensor0Json[] = "{\"name\":\"Accelerometer\",\"type\":4,\"columns\":3,\"dataFormat\":2,\"sequenceNumbers\":true,\"labels\":[\"x\", \"y\",\"z\"]}";

 /* Property structures */
 volatile static Dap_Interface_PropertyInfoType DAP_Property_Samples = {
     .NamePtr = "samples", .Type = DAP_DATA_FORMAT_UINT16, .Value = {.U16 = 0U}};


 /* DAP Interface Configuration */
 volatile static const Dap_InterfaceConfigType Test_Dap_InterfaceConfig = {
     /* Sensors */
     .SensorList    = {Dap_Sensor0Json, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                       NULL_PTR},
     .SensorCount   = 1U,
     /* Models */
     .ModelList     = {NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR},
     .ModelCount    = 0U,
     /* Properties */
     .PropertyList  = {&DAP_Property_Samples, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                       NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR},
     .PropertyCount = 1U,
     /* Inference values */
     .InfValueList  = {NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR,
                       NULL_PTR},
     .InfValueCount = 0U};

 /* ========================================================================== */
 /*                 Public Variables                                           */
 /* ========================================================================== */

 volatile Dap_InstanceType      Test_Dap_Instance;
 volatile Uart_Hal_InstanceType Test_Dap_UartInstance;


 /** \brief Initialises the sensor */
 void sensor_init()
 {
     sensor.spiReg      = SPI_0_INST;
     sensor.csPort      = SPI_PORT;
     sensor.csPin       = SPI_CS_0_PIN;
     sensor.readyPort   = SENSOR_PORT;
     sensor.readyPin    = SENSOR_RDY_1_PIN;
     sensor.sensorMode  = SENSOR_MODE_IDLE;

     DL_GPIO_disableInterrupt(SENSOR_PORT, SENSOR_RDY_1_PIN);
 }


 /* Main function */
 int main(void)
 {
     volatile uint32_t     status            = 0;
     volatile uint8        dataCounter       = 0U;
     volatile const uint16_t sampleSizes[1]  = {6U};
     volatile boolean      streamStarted     = FALSE;
     volatile boolean      isStreaming       = FALSE;
     volatile uint32_t     totalSamples      = 0;

     /* Initialize device clocks and peripherals via SysConfig */
     SYSCFG_DL_init();

     /* Initialize UART HAL */
     Uart_Hal_InitParamsType initParams;

     Uart_Hal_InitParamsSetDefault(&initParams);
     initParams.BaudRateBps = UART_0_BAUD_RATE;
     initParams.HwRegsPtr   = (UNICOMM_Inst_Regs *)UART_0_INST;
     status = Uart_Hal_Init(&Test_Dap_UartInstance, &initParams);

     if (status == UART_HAL_ERROR_NONE)
     {
         printf("UART HAL initialized successfully\n");
     }
     else
     {
         printf("ERROR: UART HAL init failed!\n");
         /* UART init failed - halt */
         while (1);
     }
     /* Enable UART interrupt in NVIC */
     NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
     NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

     Dap_InitParamsType dapParams;
     Dap_InitParamsSetDefault(&dapParams);

      /* Configure Link layer parameters */
     dapParams.LinkParams.LinkTxMode = DAP_LINK_MODE_INTERRUPT;
     dapParams.LinkParams.LinkRxMode = DAP_LINK_MODE_INTERRUPT;

     dapParams.LinkParams.UartInstancePtr = &Test_Dap_UartInstance;
     dapParams.InterfaceConfigPtr         = &Test_Dap_InterfaceConfig;

     status = Dap_Init(&Test_Dap_Instance, &dapParams);
     if (status == DAP_ERROR_NONE)
     {
         printf("DAP initialized successfully\n");
     }

     status = Dap_Open(&Test_Dap_Instance);
     if (status == DAP_ERROR_NONE)
     {
         printf("DAP opened successfully\n");
     }

     DL_GPIO_clearInterruptStatus(SENSOR_PORT, SENSOR_RDY_1_PIN);
     DL_GPIO_disableInterrupt(SENSOR_PORT, SENSOR_RDY_1_PIN);

     sensor_init();
     ADXL355_config(sensor);
     ADXL355_enableInterrupts(&sensor);
     NVIC_EnableIRQ(SENSOR_INT_IRQN);

     /* If write and read were successful, toggle LED */
     while (1)
     {
         status = Dap_Process(&Test_Dap_Instance);
         Dap_IsStreaming(&Test_Dap_Instance, &isStreaming);

         if (isStreaming == TRUE)
         {
             if (streamStarted == FALSE)
             {
                 totalSamples = Test_Dap_InterfaceConfig.PropertyList[0]->Value.U16;
                 /* sampleSizes and totalSamples args would be calculated here in end application */
                 status = Dap_StartSensorStream(&Test_Dap_Instance, sampleSizes, totalSamples);
                 if (status == DAP_ERROR_NONE)
                 {
                     dataCounter   = 0U;
                     streamStarted = TRUE;
                 }
             }
             if(streamStarted == TRUE)
             {
                 if(dataReady)
                 {
                     ADXL355_disableInterrupts(&sensor);

                     status = Dap_StreamSensorSample(&Test_Dap_Instance, 0U, (const uint8 *)input_data, 6);

                     ADXL355_enableInterrupts(&sensor);

                     NVIC_EnableIRQ(SENSOR_INT_IRQN);
                     buffCount = 0;
                     dataReady = 0;
                 }
             }
         }
         else
         {
             streamStarted = FALSE;
         }
     }
 }

 /* ========================================================================== */
 /*                 Interrupt Handlers and Callbacks                           */
 /* ========================================================================== */

 void UART_0_INST_IRQHandler(void)
 {
     Uart_Hal_InterruptHandler(&Test_Dap_UartInstance);
 }

 void Uart_Hal_TxCompleteCallback(Uart_Hal_InstanceType *instancePtr, Uart_Hal_TxStatusType *statusPtr,
                                  void *userArgsPtr)
 {
     if (instancePtr == &Test_Dap_UartInstance)
     {
         Dap_TransmitCallback(instancePtr, statusPtr, userArgsPtr);
     }
 }

 void Uart_Hal_RxCompleteCallback(Uart_Hal_InstanceType *instancePtr, Uart_Hal_RxStatusType *statusPtr,
                                  void *userArgsPtr)
 {
     if (instancePtr == &Test_Dap_UartInstance)
     {
         Dap_ReceiveCallback(instancePtr, statusPtr, userArgsPtr);
     }
 }

 /* Interrupt handler for reading SPI data from the sensor */
 void SENSOR_INT_IRQHandler(void)
 {
     switch (DL_GPIO_getPendingInterrupt(SENSOR_PORT))
     {
     case SENSOR_RDY_1_IIDX:
         if(sensor.sensorMode == SENSOR_MODE_COLLECT_DATA)
         {
             SPI_readByte(sensor, ADXL355_STATUS_ADDRESS);
             ADXL355_getData(sensor);
         }
         else
         {
             /* Incorrect interrupt  */
         }
         break;
     default:
         break;
     }
 }
