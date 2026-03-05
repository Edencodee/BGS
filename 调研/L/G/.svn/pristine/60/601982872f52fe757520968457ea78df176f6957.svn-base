/*!
    \file    main.c
    \brief   ADC_temperature_Vref

    \version 2025-05-30, V1.0.0, firmware for gd32c2x1
*/

/*
    Copyright (c) 2025, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32c2x1.h"
#include "systick.h"
#include "gd32c231c_eval.h"

__IO uint16_t adc_value[2];

float temperature;
float vref_value;

/* configure RCU peripheral */
void rcu_config(void);
/* configure ADC peripheral */
void adc_config(void);
/* ADC channel sample */
void adc_channel_sample(void);

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
    /* configure ADC peripheral */
    adc_config();

    /* USART configuration */
    gd_eval_com_init(EVAL_COM);
    printf("\r /**** ADC Demo ****/\r\n");

    while(1){
        delay_1ms(1000);
        /* ADC software trigger enable */
        adc_channel_sample();

        /* value convert */
        temperature = (0.924f - ((float)(adc_value[0])*3.3f / 4095)) * 1000 / 2.52f + 25;
        vref_value = (adc_value[1] * 3.3f / 4095);

        /* value print */
        printf("\r\n *******************");
        printf("\r\n the temperature data is %2.0f degrees Celsius", temperature);
        printf("\r\n the reference voltage data is %5.3fV", vref_value);
        printf("\r\n ******************* \r\n");
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
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCSRC_CKSYS, RCU_ADCCK_DIV10);
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
    adc_channel_length_config(ADC_INSERTED_CHANNEL, 2);

    /* ADC inserted channel config */
    adc_inserted_channel_config(0U, ADC_CHANNEL_13, ADC_SAMPLETIME_160POINT5);
    adc_inserted_channel_config(1U, ADC_CHANNEL_14, ADC_SAMPLETIME_160POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_INSERTED_CHANNEL, ADC_EXTTRIG_INSERTED_NONE); 
    adc_external_trigger_config(ADC_INSERTED_CHANNEL, ENABLE);

    /* ADC temperature and Vrefint enable */
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_TEMPSENSOR, ENABLE);
    adc_internal_channel_config(ADC_CHANNEL_INTERNAL_VREFINT, ENABLE);

    /* enable ADC interface */
    adc_enable();
    delay_1ms(1U);
}

/*!
    \brief      ADC channel sample
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_channel_sample(void)
{
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC_INSERTED_CHANNEL);

    /* wait for the end of conversion flag */
    while( (adc_flag_get(ADC_FLAG_EOIC) == RESET) || (adc_flag_get(ADC_FLAG_EOC) == RESET) );

    /* read ADC inserted data register:IDATA0��IDATA1 */
    adc_value[0] = adc_inserted_data_read(ADC_INSERTED_CHANNEL_0);
    adc_value[1] = adc_inserted_data_read(ADC_INSERTED_CHANNEL_1);

    /* clear the end of conversion flag */
    adc_flag_clear(ADC_FLAG_EOIC);
    adc_flag_clear(ADC_FLAG_EOC);
}
