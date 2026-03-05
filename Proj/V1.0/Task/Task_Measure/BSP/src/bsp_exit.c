#include "bsp_exit.h"


volatile pls_state_t g_plsState = kHigh;
/*!
    \brief      PLS_SO external interrupt initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PLS_SO_InterruptInit(void)
{
	/* enable SYSCFG clock */
	rcu_periph_clock_enable(RCU_SYSCFG);

	/* configure PLS_SO GPIO port (input, external interrupt) */
	rcu_periph_clock_enable(PLS_SO_CLK);
	gpio_mode_set(PLS_SO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, PLS_SO_PIN);
	
	/* init state from current pin level */
	g_plsState = (SET == gpio_input_bit_get(PLS_SO_PORT, PLS_SO_PIN)) ? kHigh : kLow;

	/* connect EXTI line to GPIO pin */
	syscfg_exti_line_config(PLS_SO_EXTI_PORT_SOURCE, PLS_SO_EXTI_PIN_SOURCE);
	
	/* configure EXTI line: both edge trigger */
	exti_init(PLS_SO_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	exti_interrupt_flag_clear(PLS_SO_EXTI_LINE);
	/* enable EXTI6 interrupt in NVIC */
	nvic_irq_enable(EXTI5_9_IRQn, 2U);
}

