#include "user_adc.h"
#include "systick.h"
#include "wtdint.h"

__IO uint16_t adc_value;

/*!
    \brief      configure RCU peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(SHOUT_CLK);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
    rcu_periph_clock_enable(RCU_DMAMUX);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCSRC_CKSYS, RCU_ADCCK_DIV1);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void gpio_config(void)
{
       /* config the GPIO as analog mode */
    gpio_mode_set(SHOUT_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, SHOUT_PIN);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize DMA channel 0 */
    dma_deinit(DMA_CH0);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.periph_addr  = (uint32_t)(&ADC_RDATA);
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_addr  = (uint32_t)(&adc_value);
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.number       = 1;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init_struct.request      = DMA_REQUEST_ADC;
    dma_init(DMA_CH0, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_enable(DMA_CH0);
    dma_memory_to_memory_disable(DMA_CH0);

    /* disable the DMAMUX_MUXCH0 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH0);

    /* enable DMA channel 0 */
    dma_channel_enable(DMA_CH0);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void adc_config(void)
{
		/* ADC continuous function enable */
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC scan function enable */
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC_ROUTINE_CHANNEL, 1);

    /* ADC routine channel config */
    adc_routine_channel_config(0U, SHOUT_CHANNEL, ADC_SAMPLETIME_79POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_NONE); 
    adc_external_trigger_config(ADC_ROUTINE_CHANNEL, ENABLE);

    /* ADC DMA function enable */
    adc_dma_mode_enable();

    /* enable ADC interface */
    adc_enable();
    delay_1ms(1U);

    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC_ROUTINE_CHANNEL);
}

void ADC_UserInit (void)
{
	/* RCU configuration */
	rcu_config();
	/* GPIO configuration */
	gpio_config();
	/* DMA configuration */
	dma_config();
	/* ADC configuration */
	adc_config();
}
