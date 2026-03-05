/*!
    \file    main.c
    \brief   system clock switch example

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

static void _delay(uint32_t timeout);
static void switch_system_clock_to_24m_irc48m(void);
static void switch_system_clock_to_8m_hxtal(void);
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the USART */
    gd_eval_com_init(EVAL_COM);
    printf("\r\nCK_SYS switch test demo\r\n");

    /* disable the USART */
    usart_disable(EVAL_COM);

    /* switch system clock to 24MHz by IRC48M */
    switch_system_clock_to_24m_irc48m();
    gd_eval_com_init(EVAL_COM);
    /* print out the clock frequency of system */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));

    _delay(1000);

    /* switch system clock to 8MHz by HXTAL */
    switch_system_clock_to_8m_hxtal();
    gd_eval_com_init(EVAL_COM);
    /* print out the clock frequency of system */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));

    while(1) {
    }
}

/*!
    \brief      delay function
    \param[in]  timeout: time out
    \param[out] none
    \retval     none
*/
static void _delay(uint32_t timeout)
{
    __IO uint32_t i, j;
    for(i = 0; i < timeout; i++) {
        for(j = 0; j < 500; j++) {
        }
    }
}

/*!
    \brief      switch system clock to 24M by IRC48M
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void switch_system_clock_to_24m_irc48m(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* select IRC48M as system clock source, deinitialize the RCU */
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC48MDIV_SYS);
    rcu_deinit();
    FMC_WS =(FMC_WS & (~FMC_WS_WSCNT)) | FMC_WAIT_STATE_1;
    rcu_irc48mdiv_sys_clock_config(RCU_IRC48MDIV_SYS_2);
    /* enable IRC48M */
    RCU_CTL0 |= RCU_CTL0_IRC48MEN;

    /* wait until IRC48M is stable or the startup time is longer than IRC48M_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL0 & RCU_CTL0_IRC48MSTB);
    } while((0U == stab_flag) && (IRC48M_STARTUP_TIMEOUT != timeout));
    /* if fail */
    if(0U == (RCU_CTL0 & RCU_CTL0_IRC48MSTB)) {
        while(1) {
        }
    }

    /* IRC48M is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB = AHB */
    RCU_CFG0 |= RCU_APB_CKAHB_DIV1;

    /* select IRC48M as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_IRC48MDIV_SYS;

    /* wait until IRC48M is selected as system clock */
    while((RCU_CFG0 & RCU_CFG0_SCSS) != RCU_SCSS_IRC48MDIV) {
    }
}

/*!
    \brief      switch system clock to 8M by hxtal
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void switch_system_clock_to_8m_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* select IRC48M as system clock source, deinitialize the RCU */
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC48MDIV_SYS);
    rcu_deinit();

    FMC_WS =(FMC_WS & (~FMC_WS_WSCNT)) | FMC_WAIT_STATE_0;
    /* enable HXTAL */
    RCU_CTL0 |= RCU_CTL0_HXTALEN;

    /* HXTAL is stable */
    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL0 & RCU_CTL0_HXTALSTB);
    } while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));
    /* if fail */
    if(0U == (RCU_CTL0 & RCU_CTL0_HXTALSTB)) {
        while(1) {
        }
    }

    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB = AHB */
    RCU_CFG0 |= RCU_APB_CKAHB_DIV1;

    /* select HXTAL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_HXTAL;

    /* wait until HXTAL is selected as system clock */
    while((RCU_CFG0 & RCU_CFG0_SCSS) != RCU_SCSS_HXTAL) {
    }
}
