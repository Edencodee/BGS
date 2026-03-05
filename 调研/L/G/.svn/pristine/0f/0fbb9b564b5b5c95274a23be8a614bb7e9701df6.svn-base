#include "user_spi.h"

#define ARRAYSIZE                11

uint8_t spi0_send_array[ARRAYSIZE] = {0xB3, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA};
uint32_t send_n = 0, receive_n = 0;

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
		rcu_periph_clock_enable(CAL_CLK_CLK);
    rcu_periph_clock_enable(CAL_IN_CLK);
    rcu_periph_clock_enable(RCU_DMA);
    rcu_periph_clock_enable(RCU_DMAMUX);
    rcu_periph_clock_enable(TX_SPI_CLK);
}

/*!
    \brief      configure the SPI GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* TX_SPI GPIO configuration: SCK/PB13, MISO/PB14, MOSI/PB15 */
    gpio_af_set(CAL_CLK_PORT, GPIO_AF_0, CAL_CLK_PIN );
    gpio_mode_set(CAL_CLK_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, CAL_CLK_PIN );
	
		gpio_af_set(CAL_IN_PORT, GPIO_AF_0, CAL_IN_PIN);
    gpio_mode_set(CAL_IN_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, CAL_IN_PIN);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize DMA channel 0 */
    dma_deinit(TX_DMA);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.request      = DMA_REQUEST_SPI0_TX;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr  = (uint32_t)spi0_send_array;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = ARRAYSIZE;
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(TX_SPI);
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(TX_DMA, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(TX_DMA);
    dma_memory_to_memory_disable(TX_DMA);
    /* disable the DMAMUX_MUXCH0 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH0);
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitialize SPI and the parameters */
    spi_i2s_deinit(TX_SPI);
    spi_struct_para_init(&spi_init_struct);

    /* configure TX_SPI parameter */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init_struct.nss                  = SPI_NSS_HARD;
    spi_init(TX_SPI, &spi_init_struct);

    /* configure TX_SPI byte access to FIFO */
    spi_fifo_access_size_config(TX_SPI, SPI_BYTE_ACCESS);
}

void SPI_UserInit(void)
{
	/* enable peripheral clock */
    rcu_config();
    /* configure GPIO */
    gpio_config();
    /* configure DMA */
    dma_config();
    /* configure SPI */
    spi_config();
}

void SPI_WritePD (void)
{
	/* enable TX_SPI NSS output */
    spi_nss_output_enable(TX_SPI);

    /* SPI enable */
    spi_enable(TX_SPI);

    /* SPI1_Tx DMA channel enable */
    dma_channel_enable(TX_DMA);

    spi_dma_enable(TX_SPI, SPI_DMA_TRANSMIT);

    /* wait dma transmit complete */
    while(!dma_flag_get(TX_DMA, DMA_INT_FLAG_FTF));

    /* disable TX_SPI NSS output */
    spi_nss_output_disable(TX_SPI);
}
