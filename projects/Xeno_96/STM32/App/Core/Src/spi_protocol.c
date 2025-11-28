/*************************************************************************************************
 *                                         INCLUDES                                              *
 *************************************************************************************************/
#include "spi_protocol.h"
#include "spi.h"
#include "gpio.h"
#include "Commands_spi.h"
#include <stdio.h>
#include <string.h>
/*************************************************************************************************
 *                                  LOCAL MACRO DEFINITIONS                                      *
 *************************************************************************************************/
/*************************************************************************************************
 *                                  LOCAL TYPE DEFINITIONS                                       *
 *************************************************************************************************/
/*************************************************************************************************
 *                                GLOBAL VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
/*************************************************************************************************
 *                                STATIC VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
/* Buffer used for transmission */

static uint8_t ProtocolSendBuffer[SPI_BUFFERSIZE] = {0};
// static uint8_t SpiTxBuffer[SPI_BUFFERSIZE] = {SPI_PACKET_HEADER};
static uint8_t SpiTxBuffer[SPI_BUFFERSIZE] = {0};
static uint8_t SpiRxBuffer[SPI_BUFFERSIZE] = {0};
static bool is_need_send = false;
 /*************************************************************************************************
 *                                STATIC FUNCTION DECLARATIONS                                   *
 *************************************************************************************************/
static void SetMasterIrq(void);
static void ResetMasterIrq(void);
// static void SpiDmaSetting(void);
static HAL_StatusTypeDef SpiDmaSetting(void);

 /*************************************************************************************************
 *                                GLOBAL FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/
void SpiProtocol_StartDma(void)
{
    SpiDmaSetting();
}

void SpiProtocol_SendCmd(const uint8_t cmd)
{
    if (is_need_send == false)
    {
        is_need_send = true;
        ProtocolSendBuffer[0] = cmd;
    }
    else
    {
        // todo rang buffer
    }
}

void SpiProtocol_SendBuffer(const uint8_t* data, uint16_t len)
{
    if (is_need_send == false)
    {
        is_need_send = true;
        memcpy(ProtocolSendBuffer, data, len);
    }
    else
    {
        // todo rang buffer
    }
}

void SpiProtocol_Process(void)
{
    if ((wTransferState == TRANSFER_READY) && (is_need_send == true))
    {
        wTransferState = TRANSFER_BUSY;
        SetMasterIrq();
    }

    if ((wTransferState == TRANSFER_COMPLETE) && (wTransferState != TRANSFER_BUSY))
    {
        switch(wTransferState)
        {
            case TRANSFER_COMPLETE :
            {
                if (SpiRxBuffer[0] == SPI_PACKET_HEADER)
                {
                    // printf("\n");
                    // printf("STM SPI slave ");
                    // for(uint8_t i = 0; i < 5; i++)
                    // {
                    //     printf("[%02X]",SpiRxBuffer[i]);
                    // }
                    // printf("\n");
                    Commands_HandleSpiCommand(&SpiRxBuffer[1], SPI_BUFFERSIZE);
                }
                memset(SpiTxBuffer, 0x00, sizeof(SpiTxBuffer));
                memset(SpiRxBuffer, 0x00, sizeof(SpiRxBuffer));
            }
            break;

            default:
            {
                printf("wTransferState ERROR\n");
            }
            break;
        }
        ResetMasterIrq();

        if (is_need_send == true)
        {
            is_need_send = false;
            wTransferState = TRANSFER_BUSY;
            SpiTxBuffer[0] = SPI_PACKET_HEADER;
            memcpy(&SpiTxBuffer[SPI_PACKET_DATA_START_INDEX], ProtocolSendBuffer, SPI_BUFFERSIZE - SPI_PACKET_DATA_START_INDEX);
            SpiDmaSetting();
            SetMasterIrq();
        }
        else
        {
            wTransferState = TRANSFER_READY;
            SpiDmaSetting();
        }
    }

}
 /*************************************************************************************************
 *                                STATIC FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/
static void SetMasterIrq(void)
{
    HAL_GPIO_WritePin(SLAVE_IRQ_GPIO_PORT, SLAVE_IRQ_GPIO_PIN, GPIO_PIN_SET);
}

static void ResetMasterIrq(void)
{
    HAL_GPIO_WritePin(SLAVE_IRQ_GPIO_PORT, SLAVE_IRQ_GPIO_PIN, GPIO_PIN_RESET);
}

static HAL_StatusTypeDef SpiDmaSetting(void)
{
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t*)SpiTxBuffer, (uint8_t *)SpiRxBuffer, SPI_BUFFERSIZE);
    // switch(ret)
    // {
    //     case HAL_OK:
    //     {
    //         printf("SPI DMA set comple\n");
    //         break;
    //     }
        
    //     case HAL_TIMEOUT:
    //     {
    //         printf("SPI timeout\n");
    //         break;
    //     }
            
    //     case HAL_ERROR:
    //     {
    //         printf("SPI error\n");
    //         break;
    //     }
        
    //     case HAL_BUSY:
    //     {
    //         printf("SPI busy\n");
    //         break;
    //     }
    // }
    return ret;
}