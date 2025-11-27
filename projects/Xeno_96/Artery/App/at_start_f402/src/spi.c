/*************************************************************************************************
 *                                         INCLUDES                                              *
 *************************************************************************************************/
#include "spi.h"
 /*************************************************************************************************
 *                                  LOCAL MACRO DEFINITIONS                                      *
 *************************************************************************************************/
/*************************************************************************************************
 *                                  LOCAL TYPE DEFINITIONS                                       *
 *************************************************************************************************/
/*************************************************************************************************
 *                                GLOBAL VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
uint8_t spi2_buffer_tx[SPI_BUFFER_SIZE] = {0};
uint8_t spi2_buffer_rx[SPI_BUFFER_SIZE] = {0};
 /*************************************************************************************************
 *                                STATIC VARIABLE DEFINITIONS                                    *
 *************************************************************************************************/
// static uint8_t sw = 1;
 /*************************************************************************************************
 *                                STATIC FUNCTION DECLARATIONS                                   *
 *************************************************************************************************/
static void dma_config(void);
static void spi_config(void);
static void gpio_config(void);
static void SlaveIrqGpioInit(void);
static void SlaveIrqGpioExintInit(void);

 /*************************************************************************************************
 *                                GLOBAL FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/
void Spi_init(void)
{
    dma_config();
    gpio_config();
    SlaveIrqGpioExintInit();
    SlaveIrqGpioInit();
    spi_config();
}

void Spi_ReconfigDma(void)
{
  dma_config();
}
 /*************************************************************************************************
 *                                STATIC FUNCTION DEFINITIONS                                    *
 *************************************************************************************************/

 static void dma_config(void)
{
  dma_init_type dma_init_struct;

  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  dmamux_enable(DMA1, TRUE);
  
  /* use dma1_channel1 as spi2 transmit channel */
  dma_reset(DMA1_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = SPI_BUFFER_SIZE;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init_struct.memory_base_addr = (uint32_t)spi2_buffer_tx;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(SPI2->dt);
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);
  dmamux_init(DMA1MUX_CHANNEL1, DMAMUX_DMAREQ_ID_SPI2_TX);
  
  /* use dma1_channel2 as spi2 receive channel */
  dma_reset(DMA1_CHANNEL2);
  dma_init_struct.buffer_size = SPI_BUFFER_SIZE;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init_struct.memory_base_addr = (uint32_t)spi2_buffer_rx;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(SPI2->dt);
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init(DMA1_CHANNEL2, &dma_init_struct);
  dmamux_init(DMA1MUX_CHANNEL2, DMAMUX_DMAREQ_ID_SPI2_RX);
}

static void spi_config(void)
{
  spi_init_type spi_init_struct;
  
  /* spi master initialization */
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
  spi_default_para_init(&spi_init_struct);
  
  /* dual line unidirectional full-duplex mode */
  spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI2, &spi_init_struct);
  
  /* use dma transmit and receive */
  spi_i2s_dma_transmitter_enable(SPI2,TRUE);
  spi_i2s_dma_receiver_enable(SPI2,TRUE);
  
  spi_enable(SPI2, TRUE);
}

static void gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
  gpio_default_para_init(&gpio_initstructure);
  
  /* spi master gpio initialization */
  /* spi3 cs pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_pins = GPIO_PINS_12;
  gpio_init(GPIOB, &gpio_initstructure);
  
  /* non communication time: master pull up CS pin release slave */
  SPI_MASTER_CS_HIGH;

  /* spi3 sck pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_13;
  gpio_init(GPIOB, &gpio_initstructure);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE13, GPIO_MUX_5);

  /* spi3 miso pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins = GPIO_PINS_14;
  gpio_init(GPIOB, &gpio_initstructure);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE14, GPIO_MUX_5);

  /* spi3 mosi pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins = GPIO_PINS_15;
  gpio_init(GPIOB, &gpio_initstructure);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE15, GPIO_MUX_5);
}

static void SlaveIrqGpioInit(void)
{
  gpio_init_type gpio_init_struct;
  
  /* enable the button clock */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  
  /* set default parameter */
  gpio_default_para_init(&gpio_init_struct);
  
  /* configure button pin as input with pull-up/pull-down */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_1;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_init(GPIOB, &gpio_init_struct);
}

static void SlaveIrqGpioExintInit(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOB, SCFG_PINS_SOURCE1);

  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPT;
  exint_init_struct.line_select = EXINT_LINE_1;
  // exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init_struct.line_polarity = EXINT_TRIGGER_BOTH_EDGE;
  exint_init(&exint_init_struct);

  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(EXINT1_IRQn, 0, 0);
}
