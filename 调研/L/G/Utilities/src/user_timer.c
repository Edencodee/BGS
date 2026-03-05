#include "user_timer.h"
#include <stdio.h>
#include <stdbool.h>
#include "wtdint.h"
#include "systick.h"

void EXTLS()
{
		/* enable peripheral clock */
    rcu_periph_clock_enable(EXTLS_CLK);
    /* configure clock output pin */
		gpio_af_set(EXTLS_PORT, EXTLS_AF, EXTLS_PIN);
    gpio_mode_set(EXTLS_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EXTLS_PIN);
    gpio_output_options_set(EXTLS_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_0, EXTLS_PIN);

    /* enable IRC48M clock */
    rcu_osci_on(RCU_IRC32K);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC32K)) {
        while(1) {
        }
    }
	 /* configure CKOUT0 to output 32K clock / 1*/
    rcu_ckout1_config(RCU_CKOUT1SRC_IRC32K, RCU_CKOUT1_DIV1);
}

void EXTHS()
{
/* enable peripheral clock */
    rcu_periph_clock_enable(EXTHS_CLK);
    /* configure clock output pin */
	gpio_af_set(EXTHS_PORT,EXTHS_AF,EXTHS_PIN);
    gpio_mode_set(EXTHS_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EXTHS_PIN);
    gpio_output_options_set(EXTHS_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_0, EXTHS_PIN);

    /* enable IRC48M clock */
    rcu_osci_on(RCU_IRC48M);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC48M)) {
        while(1) {
        }
    }
    /* configure CKOUT0 to output IRC48M / 8 clock */
    rcu_ckout0_config(RCU_CKOUT0SRC_IRC48M, RCU_CKOUT0_DIV8);
}