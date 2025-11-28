/*************************************************************************************************
 *                                         INCLUDES                                              *
 *************************************************************************************************/
#include "uart.h"
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
UART_HandleTypeDef UartHandle;
/*************************************************************************************************
 *                                STATIC VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
/*************************************************************************************************
 *                                STATIC FUNCTION DECLARATIONS                                   *
 *************************************************************************************************/

/*************************************************************************************************
 *                                GLOBAL FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/
void Uart_Init(void)
{
    UartHandle.Instance             = USARTx;
    UartHandle.Init.BaudRate        = 921600;
    UartHandle.Init.WordLength      = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits        = UART_STOPBITS_1;
    UartHandle.Init.Parity          = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl       = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode            = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling    = UART_OVERSAMPLING_16;
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
    {
    Error_Handler();
    }  
    if(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
    Error_Handler();
    }
    setbuf(stdout, NULL); // Disable buffering for stdout, so printf outputs immediately without waiting for newline or buffer full
}

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
 /*************************************************************************************************
 *                                STATIC FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/
