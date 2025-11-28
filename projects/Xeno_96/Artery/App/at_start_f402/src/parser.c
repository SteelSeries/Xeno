/*************************************************************************************************
 *                                         INCLUDES                                              *
 *************************************************************************************************/
#include "parser.h"
#include "spi.h"
#include "Commands.h"
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
static flag_status slave_irq_state = RESET;
static bool start_spi_comm = false;

/*************************************************************************************************
 *                                STATIC FUNCTION DECLARATIONS                                   *
 *************************************************************************************************/
/*************************************************************************************************
 *                                GLOBAL FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/
void Parser_ButtonIsr(void)
{
    /* clear interrupt pending bit */
    exint_flag_clear(EXINT_LINE_0);
    uint8_t txBuf[3] = {0x01, 0x02, 0x03};
    
    /* check input pin state */
    if(SET == gpio_input_data_bit_read(USER_BUTTON_PORT, USER_BUTTON_PIN))
    {
        Parser_SendBuffer(txBuf, sizeof(txBuf));
    }
}

void Parser_SpiIrqIsr(void)
{
    /* clear interrupt pending bit */
    exint_flag_clear(EXINT_LINE_1);
    if(gpio_input_data_bit_read(SPI_IRQ_PORT, SPI_IRQ_PIN) != slave_irq_state)
    {
      slave_irq_state = gpio_input_data_bit_read(SPI_IRQ_PORT, SPI_IRQ_PIN);
      if (slave_irq_state == SET)
      {
        start_spi_comm = true;
      }
    }
}

 void Parser_SendBuffer(const uint8_t* data, uint16_t len)
{
    if (start_spi_comm == false)
    {
        start_spi_comm = true;
        spi2_buffer_tx[0] = PARSER_PACKET_HEADER;
        memcpy(&spi2_buffer_tx[1], data, len);
        Spi_ReconfigDma();
    }
    else
    {
        // todo rang buffer
    }
}


void Parser_Process(void)
{
    if (start_spi_comm == true)
    {
        start_spi_comm = false;
        /* start communication: master pull down CS pin select slave */
        SPI_MASTER_CS_LOW;
        delay_us(5);
        /* enable spi master dma to fill and get data */
        dma_channel_enable(SPI_TX_DMA_CHANNEL, TRUE);
        dma_channel_enable(SPI_RX_DMA_CHANNEL, TRUE);
    }

        /* wait master and slave spi data receive end */
    if (dma_flag_get(SPI_RX_DMA_FDT_FLAG) == SET)
    {
        /* wait master and slave idle when communication end */
        while(spi_i2s_flag_get(SPI2, SPI_I2S_BF_FLAG) != RESET);
        /* end communication: master pull up CS pin release slave */
        delay_us(5);
        SPI_MASTER_CS_HIGH;
        memset(spi2_buffer_tx, 0x00, SPI_BUFFER_SIZE);
        Spi_ReconfigDma();

        if (spi2_buffer_rx[0] == PARSER_PACKET_HEADER)
        {
            Commands_HandleSpiCommand(&spi2_buffer_rx[1], SPI_BUFFER_SIZE);
        }

        #if defined(DEBUG_PRINTF) && 0
        // =====================debug====================
        printf("\n");
        printf("Artery SPI Master ");
        for(uint8_t i = 0; i < SPI_BUFFER_SIZE; i++)
        {
            printf("[%02X]", spi2_buffer_rx[i]);
        }
        printf("\n");
        // =====================end======================
        #endif
    }
}
/*************************************************************************************************
 *                                STATIC FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/