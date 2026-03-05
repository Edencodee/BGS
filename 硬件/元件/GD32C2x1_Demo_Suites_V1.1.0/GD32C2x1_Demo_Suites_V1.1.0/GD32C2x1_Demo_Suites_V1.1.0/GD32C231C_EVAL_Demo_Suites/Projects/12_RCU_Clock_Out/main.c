/*!
    \file    main.c
    \brief   RCU clock out demo

    \version 2025-08-08, V1.1.0, firmware for gd32c2x1
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
#include "gd32c231c_eval.h"
#include "stdio.h"

typedef enum {
    CKOUT_SYSTEMCLOCK = 0,
    CKOUT_IRC48M,
    CKOUT_IRC32K,
    CKOUT_LXTAL,
    CKOUT_HXTAL,
    CKOUT_COUNT
} clock_output_enum;

void all_led_init(void);
void all_led_off(void);
void clock_output_config(void);
void clock_output_select(uint8_t seq);

extern __IO uint8_t g_button_press_flag;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    uint8_t func_seq = 0;
    uint8_t led_seq = 0;

    /* initialize the USART */
    gd_eval_com_init(EVAL_COM);

    printf("\r\n /=========== Gigadevice Clock Output Demo ===========/ \r\n");
    printf("press user key to select clock output source \r\n");

    /* initialize the LEDs */
    all_led_init();
    /* initialize the clock output */
    clock_output_config();
    /* initialize the user key */
    gd_eval_key_init(KEY_USER, KEY_MODE_EXTI);

    while(1) {
        if(1 == g_button_press_flag) {
            /* control the led */
            all_led_off();
            gd_eval_led_on((led_typedef_enum)led_seq);
            /* select the clock output mode */
            clock_output_select(func_seq);

            func_seq++;
            func_seq %= CKOUT_COUNT;
            led_seq++;
            led_seq %= LEDn;
            g_button_press_flag = 0;
        }
    }
}

/*!
    \brief      initialize the LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void all_led_init(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}

/*!
    \brief      all LEDs turn off
    \param[in]  none
    \param[out] none
    \retval     none
*/
void all_led_off(void)
{
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
}

/*!
    \brief      configure clock output function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void clock_output_config(void)
{
    /* peripheral clock enable */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable IRC48M */
    rcu_osci_on(RCU_IRC48M);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC48M)) {
        printf("RCU_IRC48M rcu_osci_stab_wait timeout! \r\n");
        while(1) {
        }
    }

    /* enable HXTAL */
    rcu_osci_on(RCU_HXTAL);
    if(ERROR == rcu_osci_stab_wait(RCU_HXTAL)) {
        printf("RCU_HXTAL rcu_osci_stab_wait timeout! \r\n");
        while(1) {
        }
    }

    /* enable LXTAL */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
    rcu_osci_on(RCU_LXTAL);
    if(ERROR == rcu_osci_stab_wait(RCU_LXTAL)) {
        printf("RCU_LXTAL rcu_osci_stab_wait timeout! \r\n");
        while(1) {
        }
    }
    pmu_backup_write_disable();

    /* enable IRC32K */
    rcu_osci_on(RCU_IRC32K);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC32K)) {
        printf("RCU_IRC32K rcu_osci_stab_wait timeout! \r\n");
        while(1) {
        }
    }

    /* configure clock output pin */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_8);
}

/*!
    \brief      select type of clock output
    \param[in]  seq: sequence of clock output
    \param[out] none
    \retval     none
*/
void clock_output_select(uint8_t seq)
{
    switch(seq) {
    case CKOUT_SYSTEMCLOCK:
        rcu_ckout0_config(RCU_CKOUT0SRC_CKSYS, RCU_CKOUT0_DIV4);
        printf("CK_OUT: system clock, DIV: 4 \r\n");
        break;
    case CKOUT_IRC48M:
        rcu_ckout0_config(RCU_CKOUT0SRC_IRC48M, RCU_CKOUT0_DIV8);
        printf("CK_OUT: IRC48M, DIV: 8 \r\n");
        break;
    case CKOUT_IRC32K:
        rcu_ckout0_config(RCU_CKOUT0SRC_IRC32K, RCU_CKOUT0_DIV1);
        printf("CK_OUT: IRC32K, DIV: 1 \r\n");
        break;
    case CKOUT_LXTAL:
        rcu_ckout0_config(RCU_CKOUT0SRC_LXTAL, RCU_CKOUT0_DIV1);
        printf("CK_OUT: LXTAL, DIV: 1 \r\n");
        break;
    case CKOUT_HXTAL:
        rcu_ckout0_config(RCU_CKOUT0SRC_HXTAL, RCU_CKOUT0_DIV1);
        printf("CK_OUT: HXTAL, DIV: 1 \r\n");
        break;

    default:
        printf("clock output select parameter error! \r\n");
        while(1) {
        }
    }
}
