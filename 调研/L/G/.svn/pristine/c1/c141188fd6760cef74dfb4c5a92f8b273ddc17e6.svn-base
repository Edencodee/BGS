/*!
    \file    main.c
    \brief   debug TIMER2 when the MCU is in debug mode

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
#include <stdio.h>
#include "gd32c231c_eval.h"

void gpio_configuration(void);
void timer_configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* systick configuration */
    systick_config();

    /* disable timer2 hold */
    dbg_periph_disable(DBG_TIMER2_HOLD);

    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);

    /* enable DBG clock */
    rcu_periph_clock_enable(RCU_DBGMCU);

    /* configure the GPIO ports */
    gpio_configuration();

    /* configure LED GPIO port */
    gd_eval_led_init(LED1);

    /* turn off LED */
    gd_eval_led_off(LED1);

    /* configure the TIMER */
    timer_configuration();

    /* keep TIMER2 counter when the MCU is in debug mode */
    dbg_periph_enable(DBG_TIMER2_HOLD);

    while(1) {
        /* toggle LED1 output status every second */
        gd_eval_led_toggle(LED1);

        /* you can set breakpoint here, then look over the register in TIMER2 */
        delay_1ms(1000);
    }
}

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_configuration(void)
{
    /* Configure PB6 PB7 PB3(TIMER2 CH2 CH3 CH1) as alternate function */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, GPIO_PIN_6);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, GPIO_PIN_7);

    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, GPIO_PIN_3);

    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_6);
    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_7);
    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_3);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_configuration(void)
{

    /* TIMER2 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER2CLK = SystemCoreClock / 48 = 1MHz

    TIMER2 Channel1 duty cycle = 4000/ 16000  = 25%
    TIMER2 Channel2 duty cycle = 8000/ 16000  = 50%
    TIMER2 Channel3 duty cycle = 12000/ 16000 = 75% */

    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocintpara;

    rcu_periph_clock_enable(RCU_TIMER2);

    /* TIMER2  configuration */
    timer_deinit(TIMER2);
    /* TIMER2 configuration */
    timer_initpara.prescaler         = 47;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 15999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER2, &timer_initpara);

    /* CH1,CH2 and CH3 configuration in PWM mode */
    timer_ocintpara.outputstate      = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate     = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity       = TIMER_OC_POLARITY_LOW;
    timer_ocintpara.ocnpolarity      = TIMER_OCN_POLARITY_LOW;
    timer_ocintpara.ocidlestate      = TIMER_OC_IDLE_STATE_HIGH;
    timer_ocintpara.ocnidlestate     = TIMER_OCN_IDLE_STATE_HIGH;
    
    timer_channel_output_config(TIMER2, TIMER_CH_1, &timer_ocintpara);
    timer_channel_output_config(TIMER2, TIMER_CH_2, &timer_ocintpara);
    timer_channel_output_config(TIMER2, TIMER_CH_3, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_1, 3999);
    timer_channel_output_mode_config(TIMER2, TIMER_CH_1, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_2, 7999);
    timer_channel_output_mode_config(TIMER2, TIMER_CH_2, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_3, 11999);
    timer_channel_output_mode_config(TIMER2, TIMER_CH_3, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER2);
    timer_enable(TIMER2);
}
