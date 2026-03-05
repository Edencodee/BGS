#include "main.h"

__IO uint16_t adc_value;

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure NVIC peripheral */
void nvic_config(void);
/* configure ADC peripheral */
void adc_config(void);
/* configure TIMER peripheral */
void timer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();

    /* configure RCU peripheral */
    rcu_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure NVIC peripheral */
    nvic_config();
    /* configure ADC peripheral */
    adc_config();
    /* configure TIMER peripheral */
    timer_config();

    printf("\r /**** ADC Demo ****/\r\n");

    while(1){
        delay_1ms(1000);
        /* value print */
        printf("\r\n *******************");
        printf("\r\n ADC inserted channel 1 data = %d \r\n",adc_value);
        printf("\r\n ***********************************\r\n");
    }
}

/*!
    \brief      configure RCU peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable TIMER0 clock */
    rcu_periph_clock_enable(RCU_TIMER0);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCSRC_CKSYS, RCU_ADCCK_DIV8);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);
}

/*!
    \brief        configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(ADC_IRQn, 0);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC continuous function enable */
    adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE);
    /* ADC scan function enable */
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC_INSERTED_CHANNEL, 4);

    /* ADC inserted channel config */
    adc_inserted_channel_config(1U, ADC_CHANNEL_1, ADC_SAMPLETIME_7POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_INSERTED_CHANNEL, ADC_EXTTRIG_INSERTED_T0_CH3); 
    adc_external_trigger_config(ADC_INSERTED_CHANNEL, ENABLE);

    /* clear the ADC flag */
    adc_interrupt_flag_clear(ADC_INT_FLAG_EOC);
    adc_interrupt_flag_clear(ADC_INT_FLAG_EOIC);
    /* enable ADC interrupt */
    adc_interrupt_enable(ADC_INT_EOIC);

    /* enable ADC interface */
    adc_enable();
    delay_1ms(1U);
}

/*!
    \brief      configure TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* ----------------------------------------------------------------------------------
    TIMER0 configuration: generate 1 PWM signals:
    TIMER0CLK = SystemCoreClock / 4800 = 10kHz

    TIMER0 channel3 duty cycle = (5000/ 10000)* 100 = 50%

    Select TIMER0 channel3 as ADC trigger input
    ----------------------------------------------------------------------- -------------*/
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER0);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 4799;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* CH3 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocintpara);

    /* CH3 configuration in PWM mode0,duty cycle 50% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, 4999);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

    /* enable TIMER primary output function */
    timer_primary_output_config(TIMER0, ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    /* TIMER1 enable */
    timer_enable(TIMER0);
}

/* retarget the C library printf function to the USART ------------- */
int fputc(int ch, FILE *f)
{
 SEGGER_RTT_PutChar(0, ch);
 return (ch);
}
/* ----------------------------------------------------------------- */