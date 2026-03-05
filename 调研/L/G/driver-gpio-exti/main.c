#include "main.h"

void gd_eval_key_init();
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	gd_eval_key_init();

    while(1) {
    }
}

/*!
    \brief      configure key
    \param[in]  keynum: specify the key to be configured
      \arg        KEY_WAKEUP: wakeup key
      \arg        KEY_TAMPER: tamper key
    \param[in]  keymode: specify button mode
      \arg        KEY_MODE_GPIO: key will be used as simple IO
      \arg        KEY_MODE_EXTI: key will be connected to EXTI line with interrupt
    \param[out] none
    \retval     none
*/
void gd_eval_key_init()
{
    /* enable the key clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* configure button pin as input */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_4);

        /* enable and set key EXTI interrupt to the lowest priority */
        nvic_irq_enable(EXTI4_IRQn, 2U);
        /* connect key EXTI line to key GPIO pin */
        syscfg_exti_line_config(EXTI_SOURCE_GPIOA,EXTI_SOURCE_PIN4);
        /* configure key EXTI line */
        exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
        exti_interrupt_flag_clear(EXTI_4);
}

/* retarget the C library printf function to the USART ------------- */
int fputc(int ch, FILE *f)
{
 SEGGER_RTT_PutChar(0, ch);
 return (ch);
}
/* ----------------------------------------------------------------- */