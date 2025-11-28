
#pragma once
/*************************************************************************************************
 *                                          INCLUDES                                             *
 *************************************************************************************************/
#include "main.h"

/*************************************************************************************************
 *                                   GLOBAL MACRO DEFINITIONS                                    *
 *************************************************************************************************/
/* Definition for SPIx clock resources */
#define SPIx                                SPI1
#define SPIx_CLK_ENABLE()                 __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_CS_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                 __HAL_RCC_DMA2_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                      GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT                GPIOA
#define SPIx_SCK_AF                       GPIO_AF5_SPI1

#define SPIx_MISO_PIN                     GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT               GPIOA
#define SPIx_MISO_AF                      GPIO_AF5_SPI1

#define SPIx_MOSI_PIN                     GPIO_PIN_5
#define SPIx_MOSI_GPIO_PORT               GPIOB
#define SPIx_MOSI_AF                      GPIO_AF5_SPI1

#define SPIx_CS_PIN                       GPIO_PIN_4
#define SPIx_CS_GPIO_PORT                 GPIOA
#define SPIx_CS_AF                        GPIO_AF5_SPI1

/* Size of buffer */
#define SPI_BUFFERSIZE                      32

/* Definition for SPIx's DMA */
#define SPIx_TX_DMA_STREAM               DMA2_Stream3
#define SPIx_RX_DMA_STREAM               DMA2_Stream2
#define SPIx_TX_DMA_CHANNEL              DMA_CHANNEL_3
#define SPIx_RX_DMA_CHANNEL              DMA_CHANNEL_3

/* Definition for SPIx's NVIC */
#define SPIx_DMA_TX_IRQn                 DMA2_Stream3_IRQn
#define SPIx_DMA_RX_IRQn                 DMA2_Stream2_IRQn

#define SPIx_DMA_TX_IRQHandler           DMA2_Stream3_IRQHandler
#define SPIx_DMA_RX_IRQHandler           DMA2_Stream2_IRQHandler
/*************************************************************************************************
 *                                    GLOBAL TYPE DEFINITIONS                                    *
 *************************************************************************************************/
enum {
	TRANSFER_READY,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR,
	TRANSFER_BUSY,
};
/*************************************************************************************************
 *                                  GLOBAL VARIABLE DECLARATIONS                                 *
 *************************************************************************************************/
extern SPI_HandleTypeDef SpiHandle;
extern __IO uint32_t wTransferState;


/*************************************************************************************************
 *                                  GLOBAL FUNCTION DECLARATIONS                                 *
 *************************************************************************************************/
void Spi_Init(void);

