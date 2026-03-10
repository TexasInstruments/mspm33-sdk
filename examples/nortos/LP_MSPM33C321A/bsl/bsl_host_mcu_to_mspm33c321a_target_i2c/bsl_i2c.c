// Generic MSP430 Device Include
#include <bsl_i2c.h>
#include <i2c.h>

#include "stdio.h"
#include "string.h"
#include "ti_msp_dl_config.h"

void Host_BSL_software_trigger(void)
{
    i2c_error_t i2c_ack;
    uint8_t buf[1] = {0x22};
    i2c_ack        = I2C_writeBuffer(buf, 1);
}

/*
 * Turn on the error LED
 */
void TurnOnErrorLED(void)
{
    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LED_Error_PIN);
}
//*****************************************************************************
//
// ! Host_BSL_Connection
// ! Need to send first to build connection with target
//
//*****************************************************************************
uint8_t test_G;
BSL_error_t Host_BSL_Connection(void)
{
    BSL_error_t bsl_err = eBSL_success;
    i2c_error_t i2c_ack;
    uint32_t ui32CRC;

    BSL_TX_buffer[0] = (uint8_t) PACKET_HEADER;
    BSL_TX_buffer[1] = LSB(CMD_BYTE);
    BSL_TX_buffer[2] = 0x00;
    BSL_TX_buffer[3] = CMD_CONNECTION;

    // Calculate CRC on the PAYLOAD (CMD + Data)
    ui32CRC = softwareCRC(&BSL_TX_buffer[3], CMD_BYTE);
    // Insert the CRC into the packet
    *(uint32_t *) &BSL_TX_buffer[HDR_LEN_CMD_BYTES] = ui32CRC;

    i2c_ack = I2C_writeBuffer(BSL_TX_buffer, HDR_LEN_CMD_BYTES + CRC_BYTES);
    I2C_readBuffer(BSL_RX_buffer, 1);
    if (i2c_ack != uart_noError) {
        TurnOnErrorLED();
    }
    return (bsl_err);
}

//*****************************************************************************
// ! Host_BSL_GetID
// ! Need to send when build connection to get RAM BSL_RX_buffer size and other information
//
//*****************************************************************************
BSL_error_t Host_BSL_GetID(void)
{
    BSL_error_t bsl_err = eBSL_success;
    i2c_error_t i2c_ack;
    uint32_t ui32CRC;

    BSL_TX_buffer[0] = (uint8_t) PACKET_HEADER;
    BSL_TX_buffer[1] = LSB(CMD_BYTE);
    BSL_TX_buffer[2] = 0x00;
    BSL_TX_buffer[3] = CMD_GET_ID;

    // Calculate CRC on the PAYLOAD (CMD + Data)
    ui32CRC = softwareCRC(&BSL_TX_buffer[3], CMD_BYTE);
    // Insert the CRC into the packet
    *(uint32_t *) &BSL_TX_buffer[HDR_LEN_CMD_BYTES] = ui32CRC;

    // Write the packet to the target
    i2c_ack = I2C_writeBuffer(BSL_TX_buffer, HDR_LEN_CMD_BYTES + CRC_BYTES);
    if (i2c_ack != uart_noError) {
        TurnOnErrorLED();
    }
    I2C_readBuffer(BSL_RX_buffer, 1);
    delay_cycles(1000);
    I2C_readBuffer(BSL_RX_buffer, HDR_LEN_CMD_BYTES + ID_BACK + CRC_BYTES);
    BSL_MAX_BUFFER_SIZE = 0;
    BSL_MAX_BUFFER_SIZE =
        *(uint16_t *) &BSL_RX_buffer[HDR_LEN_CMD_BYTES + ID_BACK - 14];
    return (bsl_err);
}
//*****************************************************************************
// ! Unlock BSL for programming
// ! If first time, assume blank device.
// ! This will cause a mass erase and destroy previous password.
// ! When programming complete, issue BSL_readPassword to retrieve new one.
//
//*****************************************************************************
BSL_error_t Host_BSL_loadPassword(uint8_t *pPassword)
{
    BSL_error_t bsl_err = eBSL_success;
    i2c_error_t i2c_ack;
    uint32_t ui32CRC;

    BSL_TX_buffer[0] = (uint8_t) PACKET_HEADER;
    BSL_TX_buffer[1] = LSB(PASSWORD_SIZE + CMD_BYTE);
    BSL_TX_buffer[2] = 0x00;
    BSL_TX_buffer[3] = CMD_RX_PASSWORD;

    memcpy(&BSL_TX_buffer[4], pPassword, PASSWORD_SIZE);

    // Calculate CRC on the PAYLOAD (CMD + Data)
    ui32CRC = softwareCRC(&BSL_TX_buffer[3], PASSWORD_SIZE + CMD_BYTE);

    // Insert the CRC into the packet
    *(uint32_t *) &BSL_TX_buffer[HDR_LEN_CMD_BYTES + PASSWORD_SIZE] = ui32CRC;

    // Write the packet to the target
    i2c_ack = I2C_writeBuffer(
        BSL_TX_buffer, HDR_LEN_CMD_BYTES + PASSWORD_SIZE + CRC_BYTES);
    if (i2c_ack != uart_noError) {
        TurnOnErrorLED();
    }

    bsl_err = Host_BSL_getResponse();

    return (bsl_err);
}

BSL_error_t Host_BSL_MemoryReadBack(uint32_t start_addr, uint32_t length)
{
    BSL_error_t bsl_err = eBSL_success;
    i2c_error_t i2c_ack;
    uint32_t ui32CRC;

    BSL_TX_buffer[0] = (uint8_t) PACKET_HEADER;
    BSL_TX_buffer[1] = LSB(CMD_BYTE + ADDRS_BYTES + ADDRS_BYTES);
    BSL_TX_buffer[2] = 0x00;
    BSL_TX_buffer[3] = CMD_MEMORY_READBACK;
    *(uint32_t *) &BSL_TX_buffer[HDR_LEN_CMD_BYTES] = start_addr;

    memcpy((uint8_t *) &BSL_TX_buffer[HDR_LEN_CMD_BYTES + ADDRS_BYTES],
        (uint8_t *) &length, ADDRS_BYTES);

    // Calculate CRC on the PAYLOAD (CMD + Data)
    ui32CRC =
        softwareCRC(&BSL_TX_buffer[3], ADDRS_BYTES + ADDRS_BYTES + CMD_BYTE);

    // Insert the CRC into the packet
    *(uint32_t *) &BSL_TX_buffer[HDR_LEN_CMD_BYTES + ADDRS_BYTES +
                                 ADDRS_BYTES] = ui32CRC;

    // Write the packet to the target
    i2c_ack = I2C_writeBuffer(BSL_TX_buffer,
        HDR_LEN_CMD_BYTES + ADDRS_BYTES + ADDRS_BYTES + CRC_BYTES);
    if (i2c_ack != uart_noError) {
        TurnOnErrorLED();
    }

    I2C_readBuffer(
        BSL_RX_buffer, ACK_BYTE + HDR_LEN_CMD_BYTES + length + CRC_BYTES);
    bsl_err = BSL_RX_buffer[ACK_BYTE + HDR_LEN_CMD_BYTES + MSG - 1];

    return (bsl_err);
}

//*****************************************************************************
// ! Host_BSL_MassErase
// ! Need to do mess erase before write new image
//
//*****************************************************************************
BSL_error_t Host_BSL_MassErase(void)
{
    BSL_error_t bsl_err = eBSL_success;
    i2c_error_t i2c_ack;
    uint32_t ui32CRC;

    BSL_TX_buffer[0] = (uint8_t) PACKET_HEADER;
    BSL_TX_buffer[1] = LSB(CMD_BYTE);
    BSL_TX_buffer[2] = 0x00;
    BSL_TX_buffer[3] = CMD_MASS_ERASE;

    // Calculate CRC on the PAYLOAD (CMD + Data)
    ui32CRC = softwareCRC(&BSL_TX_buffer[3], CMD_BYTE);
    // Insert the CRC into the packet
    *(uint32_t *) &BSL_TX_buffer[HDR_LEN_CMD_BYTES] = ui32CRC;

    // Write the packet to the target
    i2c_ack = I2C_writeBuffer(BSL_TX_buffer, HDR_LEN_CMD_BYTES + CRC_BYTES);
    if (i2c_ack != uart_noError) {
        TurnOnErrorLED();
    }
    delay_cycles(2000);
    bsl_err = Host_BSL_getResponse();
    return (bsl_err);
}

//*****************************************************************************
//
// ! Host_BSL_writeMemory
// ! Writes memory section to target
//
//*****************************************************************************
BSL_error_t Host_BSL_writeMemory(
    uint32_t addr, const uint8_t *data, uint32_t len)
{
    BSL_error_t bsl_err = eBSL_success;
    i2c_error_t i2c_ack;
    uint16_t ui16DataLength;
    uint16_t ui16PayloadSize;
    uint16_t ui16PacketSize;
    uint32_t ui32CRC;
    uint16_t ui16BytesToWrite = len;
    uint32_t TargetAddress    = addr;

    //  pSection->checksum = softwareCRC(pSection->pMemory, pSection->mem_size);

    while (ui16BytesToWrite > 0) {
        //        delay_cycles(1000000); //allow target deal with the packet send before

        if (ui16BytesToWrite >= MAX_PAYLOAD_DATA_SIZE)
            ui16DataLength = MAX_PAYLOAD_DATA_SIZE;
        else
            ui16DataLength = ui16BytesToWrite;

        ui16BytesToWrite = ui16BytesToWrite - ui16DataLength;

        // Add (1byte) command + (4 bytes)ADDRS = 5 bytes to the payload
        ui16PayloadSize = (CMD_BYTE + ADDRS_BYTES + ui16DataLength);

        BSL_TX_buffer[0] = PACKET_HEADER;
        BSL_TX_buffer[1] =
            LSB(ui16PayloadSize);  // typically 4 + MAX_PAYLOAD SIZE
        BSL_TX_buffer[2] = MSB(ui16PayloadSize);
        BSL_TX_buffer[3] = (uint8_t) CMD_PROGRAMDATA;
        *(uint32_t *) &BSL_TX_buffer[HDR_LEN_CMD_BYTES] = TargetAddress;

        // Bump up the target address by 2x the number of bytes sent for the next packet
        TargetAddress += ui16DataLength;

        // Copy the data into the BSL_RX_buffer
        memcpy(&BSL_TX_buffer[HDR_LEN_CMD_BYTES + ADDRS_BYTES], data,
            ui16DataLength);

        data += ui16DataLength;

        // Calculate CRC on the PAYLOAD
        ui32CRC = softwareCRC(&BSL_TX_buffer[3], ui16PayloadSize);

        // Calculate the packet length
        ui16PacketSize = HDR_LEN_CMD_BYTES + ADDRS_BYTES + ui16DataLength;

        // Insert the CRC into the packet at the end
        *(uint32_t *) &BSL_TX_buffer[ui16PacketSize] = ui32CRC;

        // Write the packet to the target
        i2c_ack = I2C_writeBuffer(BSL_TX_buffer, ui16PacketSize + CRC_BYTES);
        if (i2c_ack != uart_noError) {
            TurnOnErrorLED();
        }

        // Check operation was complete
        bsl_err = Host_BSL_getResponse();
        // I2C_readBuffer(BSL_RX_buffer, 1);
        // delay_cycles(1000);
        // I2C_readBuffer(BSL_RX_buffer, (HDR_LEN_CMD_BYTES + MSG + CRC_BYTES));

        if (bsl_err != eBSL_success) break;

    }  // end while

    return (bsl_err);
}

//*****************************************************************************
// ! Host_BSL_StartApp
// ! Start the new application
//
//*****************************************************************************
BSL_error_t Host_BSL_StartApp(void)
{
    BSL_error_t bsl_err = eBSL_success;
    i2c_error_t i2c_ack;
    uint32_t ui32CRC;

    BSL_TX_buffer[0] = (uint8_t) PACKET_HEADER;
    BSL_TX_buffer[1] = LSB(CMD_BYTE);
    BSL_TX_buffer[2] = 0x00;
    BSL_TX_buffer[3] = CMD_START_APP;

    // Calculate CRC on the PAYLOAD (CMD + Data)
    ui32CRC = softwareCRC(&BSL_TX_buffer[3], CMD_BYTE);
    // Insert the CRC into the packet
    *(uint32_t *) &BSL_TX_buffer[HDR_LEN_CMD_BYTES] = ui32CRC;

    // Write the packet to the target
    i2c_ack = I2C_writeBuffer(BSL_TX_buffer, HDR_LEN_CMD_BYTES + CRC_BYTES);
    I2C_readBuffer(BSL_RX_buffer, 1);
    if (i2c_ack != uart_noError) {
        TurnOnErrorLED();
    }
    return (bsl_err);
}

//*****************************************************************************
//
// ! softwareCRC
// ! Can be used on MSP430 and non-MSP platforms
// ! This functions computes the 16-bit CRC (same as BSL on MSP target)
//
//*****************************************************************************
#define CRC32_POLY 0xEDB88320
uint32_t softwareCRC(const uint8_t *data, uint32_t length)
{
    uint16_t bufferIndex;

    DL_CRCP_setSeed32(CRC_Regs, BSL_CRC_SEED);

    for (bufferIndex = (uint16_t) 0; bufferIndex < length; bufferIndex++) {
        DL_CRCP_feedData8(CRC_Regs, data[bufferIndex]);
    }
    return DL_CRCP_getResult32(CRC_Regs);
}

//*****************************************************************************
//
// ! Host_BSL_getResponse
// ! For those function calls that don't return specific data.
// ! Returns errors.
//
//*****************************************************************************
BSL_error_t Host_BSL_getResponse(void)
{
    BSL_error_t bsl_err = eBSL_success;

    I2C_readBuffer(
        BSL_RX_buffer, (ACK_BYTE + HDR_LEN_CMD_BYTES + MSG + CRC_BYTES));
    //   Get ACK value
    bsl_err = BSL_RX_buffer[ACK_BYTE + HDR_LEN_CMD_BYTES + MSG - 1];
    //   Return ACK value
    return (bsl_err);
}
