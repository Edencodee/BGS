/*!
    \file    main.c
    \brief   ADC_analog_watchdog0

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
#define BOARD_ADC_CHANNEL   ADC_CHANNEL_2
#define ADC_GPIO_PORT_RCU   RCU_GPIOA
#define ADC_GPIO_PORT       GPIOA
#define ADC_GPIO_PIN        GPIO_PIN_2

#define ADC_WATCHDOG_HT     0x0A00
#define ADC_WATCHDOG_LT     0x0400

__IO uint16_t adc_value;
__IO uint16_t WDE_FALG_data;

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure NVIC peripheral */
void nvic_config(void);
/* configure ADC peripheral */
void adc_config(void);

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

    /* initialize LED1 */
    gd_eval_led_init(LED1);

    /* configure RCU peripheral */
    rcu_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure NVIC peripheral */
    nvic_config();
    /* configure ADC peripheral */
    adc_config();

    /* USART configuration */
    gd_eval_com_init(EVAL_COM);
    printf("\r /**** ADC Demo ****/\r\n");

    while(1){
        delay_1ms(1000);
        adc_value = adc_routine_data_read();
        printf("\r\n *******************");
        printf("\r\n ADC watchdog low threshold: %04X \r\n",ADC_WATCHDOG_LT);
        printf("\r\n ADC watchdog hgih threshold: %04X \r\n",ADC_WATCHDOG_HT);
        printf("\r\n ADC routine channel data = %04X \r\n",adc_value);
        printf("\r\n ***********************************\r\n");
        if(1 == WDE_FALG_data){
            /* turn off LED2 */
            if((adc_value < ADC_WATCHDOG_HT) && (adc_value > ADC_WATCHDOG_LT)){
                WDE_FALG_data = 0;
                gd_eval_led_off(LED1);
            }
        }
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
    rcu_periph_clock_enable(ADC_GPIO_PORT_RCU);
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
    gpio_mode_set(ADC_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_GPIO_PIN);
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
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC scan function enable */
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC_ROUTINE_CHANNEL, 1);

    /* ADC routine channel config */
    adc_routine_channel_config(0U, BOARD_ADC_CHANNEL, ADC_SAMPLETIME_79POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_NONE); 
    adc_external_trigger_config(ADC_ROUTINE_CHANNEL, ENABLE);

    /* ADC analog watchdog threshold config */
    adc_watchdog0_threshold_config(ADC_WATCHDOG_LT, ADC_WATCHDOG_HT);
    /* ADC analog watchdog single channel config */
    adc_watchdog0_single_channel_enable(BOARD_ADC_CHANNEL);

    /* clear the ADC flag */
    adc_interrupt_flag_clear(ADC_INT_FLAG_WD0E);
    /* ADC interrupt config */
    adc_interrupt_enable(ADC_INT_WD0E);

    /* enable ADC interface */
    adc_enable();
    delay_1ms(1U);

    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC_ROUTINE_CHANNEL);
}
