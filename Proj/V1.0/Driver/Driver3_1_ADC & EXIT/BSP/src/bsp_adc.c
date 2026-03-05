#include "bsp_adc.h"

/* ADC value buffer -------------------------------------------------- */
uint16_t adc_value_buf[ADC_BUF_LEN] = {0};
uint8_t adc_buf_index = 0;

/* ------------------------------------------------------------------- */

/*!
    \brief      ADC user initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ADC_UserInit(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    
    /* reset ADC */
    adc_deinit();
    
    /* ADC scan mode configuration */
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);
    
    /* ADC data alignment configuration */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    
    /* ADC channel length configuration, 1 channel */
    adc_channel_length_config(ADC_ROUTINE_CHANNEL, 1U);
    
    /* ADC regular channel configuration: PA1 (ADC_IN1) */
    adc_routine_channel_config(0U, ADC_CHANNEL_1, ADC_SAMPLETIME_12POINT5);
    
    /* ADC external trigger source configuration - EXTI line 11 */
    adc_external_trigger_config(ADC_ROUTINE_CHANNEL, ENABLE);
    adc_external_trigger_source_config(ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_EXTI_11);
    
    /* ADC interrupt configuration */
    adc_interrupt_flag_clear(ADC_INT_FLAG_EOC);
    adc_interrupt_enable(ADC_INT_EOC);
    
    /* enable ADC */
    adc_enable();
}

/*!
    \brief      get ADC converted value
    \param[in]  none
    \param[out] none
    \retval     ADC converted value
*/
uint16_t ADC_GetValue(void)
{
    return adc_routine_data_read();
}
