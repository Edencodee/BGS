#include "bsp_adc.h"

/* ADC value buffer -------------------------------------------------- */
volatile uint8_t g_sampleIndex = 0;
uint16_t g_adcCaptureBuffer[ADC_BUF_LEN] = {0};
volatile uint8_t g_isSampleDone = 0U;
/* ------------------------------------------------------------------- */

void RCU_Config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(ADC_PIN_CLK);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
    rcu_periph_clock_enable(RCU_DMAMUX);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCSRC_CKSYS, RCU_ADCCK_DIV8);
}


void GPIO_Config(void)
{
    /* config the GPIO as analog mode */
    gpio_mode_set(ADC_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_PIN);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA_Config(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize DMA channel 0 */
    dma_deinit(ADC_DMA_CHANNEL);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.request      = DMA_REQUEST_ADC;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.periph_addr  = (uint32_t)(&ADC_RDATA);
    dma_init_struct.number       = ADC_BUF_LEN;
    dma_init_struct.memory_addr  = (uint32_t)g_adcCaptureBuffer;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.priority     = DMA_PRIORITY_HIGH;

    dma_init(ADC_DMA_CHANNEL, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(ADC_DMA_CHANNEL);
    /* disable the DMAMUX_MUXCH0 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH0);
    /* enable DMA channel 0 */
    //dma_channel_enable(ADC_DMA_CHANNEL);
    /* enable DMA transfer complete interrupt */
    dma_interrupt_enable(ADC_DMA_CHANNEL, DMA_INT_FTF);
    nvic_irq_enable(DMA_Channel0_IRQn, 1U);
}

void ADC_Config(void)
{
    /* ADC continuous function enable */
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC scan function enable */
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC_ROUTINE_CHANNEL, 1);

    /* ADC routine channel config */
    adc_routine_channel_config(0U, ADC_CHANNEL_1, ADC_SAMPLETIME_79POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_NONE);
    adc_external_trigger_config(ADC_ROUTINE_CHANNEL, DISABLE);

    /* ADC DMA function enable */
    adc_dma_mode_enable();

    /* enable ADC interface */
    adc_enable();
    delay_1ms(1U);
}

void DMA_Channel0_IRQHandler(void)
{
    if(dma_interrupt_flag_get(DMA_CH0, DMA_INT_FLAG_FTF))
    {
        dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_FTF);
        ADC_Sampling_Stop();
        g_isSampleDone = 1;
    }
}


void ADC_UserInit(void)
{
	//printf("adc init\r\n");
	RCU_Config();
	GPIO_Config();
	DMA_Config();
	ADC_Config();
}

void ADC_Sampling_Start(void)
{
    //g_isSampleDone = 0;
    dma_channel_disable(ADC_DMA_CHANNEL);
    dma_transfer_number_config(ADC_DMA_CHANNEL, ADC_BUF_LEN);
	dma_channel_enable(ADC_DMA_CHANNEL);
    adc_enable();
    adc_software_trigger_enable(ADC_ROUTINE_CHANNEL);
}

void ADC_Sampling_Stop(void)
{
    adc_disable();
}