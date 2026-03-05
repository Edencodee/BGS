/*!
    \file    main.c
    \brief   Deepsleep wakeup

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
#include <stdio.h>
#include "gd32c231c_eval.h"
#include "systick.h"

extern __IO uint8_t counter0;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();

    gd_eval_led_init(LED1);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* USART configuration the CK_IRC16M as USART clock */
    rcu_usart_clock_config(IDX_USART0, RCU_USART0SRC_IRC48MDIV_PER);
    gd_eval_com_init(EVAL_COM);

    nvic_irq_enable(USART0_WKUP_IRQn, 0);
    /* USART1 Wakeup EXTI line configuretion */
    exti_init(EXTI_22, EXTI_INTERRUPT, EXTI_TRIG_RISING);

    delay_1ms(2000);

    /* use start bit wakeup MCU */
    usart_wakeup_mode_config(EVAL_COM, USART_WUM_STARTB);

    /* enable USART */
    usart_enable(EVAL_COM);
    /* ensure USART is enabled */
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_REA)) {
    }
    /* check USART is not transmitting */
    while(SET == usart_flag_get(EVAL_COM, USART_FLAG_BSY)) {
    }

    usart_wakeup_enable(EVAL_COM);
    /* enable the WUIE interrupt */
    usart_interrupt_enable(EVAL_COM, USART_INT_WU);

    /* enable PWU APB clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enter deep-sleep mode */
    pmu_to_deepsleepmode(WFI_CMD, PMU_DEEPSLEEP1);

    /* wait a WUIE interrupt event */
    while(0x00 == counter0);

    /* disable USART peripheral in deepsleep mode */
    usart_wakeup_disable(EVAL_COM);

    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_RBNE));
    usart_data_receive(EVAL_COM);

    usart_receive_config(EVAL_COM, USART_RECEIVE_ENABLE);

    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TC));

    /* disable the USART */
    usart_disable(EVAL_COM);

    systick_config();

    while(1) {
    }
}

/*!
    \brief      LED spark
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t time_delay = 0;

    if(0x00 != time_delay) {
        if(time_delay < 500) {
            /* light on */
            gd_eval_led_on(LED1);
        } else {
            /* light off */
            gd_eval_led_off(LED1);
        }
        time_delay--;
    } else {
        time_delay = 1000;
    }
}
