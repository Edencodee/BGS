#include "bsp_adc.h"
#include "systick.h"

/* ADC value buffer -------------------------------------------------- */
__IO int16_t g_A_Buffer[SAMPLE_POINT] = {0};
__IO int16_t g_B_Buffer[SAMPLE_POINT] = {0};
/* ------------------------------------------------------------------- */


/*!
    \brief      configure RCU peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RCU_Config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(ADC_PIN_CLK);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
    rcu_periph_clock_enable(RCU_DMAMUX);
    /* TIMER2 clock enable */
    rcu_periph_clock_enable(RCU_TIMER2);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCSRC_CKSYS, RCU_ADCCK_DIV4);
}


/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void GPIO_Config(void)
{
    /* config the GPIO as analog mode */
    gpio_mode_set(ADC_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_PIN);
}


void ADC_TIMER_Config(void)
{
    /* TIMER2 configuration */
    timer_parameter_struct timer_initpara;
    timer_deinit(TIMER2);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 0U;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 47U;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0U;
    timer_init(TIMER2, &timer_initpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER2);

    /* TIMER2 trigger output select: update event */
    timer_master_output_trigger_source_select(TIMER2, TIMER_TRI_OUT_SRC_UPDATE);

    /* TIMER2 enable counter */
    //timer_enable(TIMER2);

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
    dma_init_struct.number       = SAMPLE_POINT;
    dma_init_struct.memory_addr  = (int32_t)g_A_Buffer;
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

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ADC_Config(void)
{
    /* ADC deinit */
    adc_deinit();
    /* ADC continuous function disable */
    adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE);
    /* ADC scan function disable */
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC_ROUTINE_CHANNEL, 1U);

    /* ADC routine channel config */
    adc_routine_channel_config(0U, ADC_CHANNEL_1, ADC_SAMPLETIME_19POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T2_TRGO);
    adc_external_trigger_config(ADC_ROUTINE_CHANNEL, ENABLE);

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
        ADC_StopSampling();
        g_isSampleDone = true;
    }
}

/*!
    \brief      ADC user initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ADC_UserInit(void)
{
	//printf("adc init\r\n");
	RCU_Config();
	GPIO_Config();
    ADC_TIMER_Config();
	DMA_Config();
	ADC_Config();
}

void ADC_StartSampling(void)
{
    dma_channel_disable(ADC_DMA_CHANNEL);
    dma_transfer_number_config(ADC_DMA_CHANNEL, SAMPLE_POINT);
	dma_channel_enable(ADC_DMA_CHANNEL);
    /* ADC already enabled in ADC_Config(), no need to enable again */
    timer_enable(TIMER2);
}

void ADC_StopSampling(void)
{
   // adc_disable();
    timer_disable(TIMER2);
}

void ADC_SwitchBuffer(int16_t *g_adcCaptureBuffer)
{
    /* Swap buffers: minimal latency update */
    dma_channel_disable(ADC_DMA_CHANNEL);
    dma_memory_address_config(ADC_DMA_CHANNEL, (int32_t)g_adcCaptureBuffer);
    dma_transfer_number_config(ADC_DMA_CHANNEL, SAMPLE_POINT);
    dma_channel_enable(ADC_DMA_CHANNEL);
}
