
#pragma once
/*************************************************************************************************
 *                                          INCLUDES                                             *
 *************************************************************************************************/
#include "at32f402_405_board.h"

 /*************************************************************************************************
 *                                   GLOBAL MACRO DEFINITIONS                                    *
 *************************************************************************************************/
#define SPI_MASTER_CS_HIGH      gpio_bits_set(GPIOB, GPIO_PINS_12)
#define SPI_MASTER_CS_LOW       gpio_bits_reset(GPIOB, GPIO_PINS_12)
#define SPI_BUFFER_SIZE         32
#define SPI_IRQ_PORT            GPIOB
#define SPI_IRQ_PIN             GPIO_PINS_1
#define SPI_TX_DMA_CHANNEL      DMA1_CHANNEL1
#define SPI_RX_DMA_CHANNEL      DMA1_CHANNEL2
#define SPI_RX_DMA_FDT_FLAG     DMA1_FDT2_FLAG

/*************************************************************************************************
 *                                    GLOBAL TYPE DEFINITIONS                                    *
 *************************************************************************************************/

/*************************************************************************************************
 *                                  GLOBAL VARIABLE DECLARATIONS                                 *
 *************************************************************************************************/
extern uint8_t spi2_buffer_tx[SPI_BUFFER_SIZE];
extern uint8_t spi2_buffer_rx[SPI_BUFFER_SIZE];
/*************************************************************************************************
 *                                  GLOBAL FUNCTION DECLARATIONS                                 *
 ************************************************************************************************/
void Spi_init(void);
void Spi_ReconfigDma(void);


