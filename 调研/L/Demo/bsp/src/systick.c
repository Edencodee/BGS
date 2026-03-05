#include "systick.h"

volatile static uint32_t delay;
volatile static uint32_t ticks = 0;

/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_config(void)
{
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

	/* setup systick timer for 1000Hz interrupts */
	if (SysTick_Config(SystemCoreClock / 1000U)) {
		/* capture error */
		while (1) {
		}
	}
	/* configure the systick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x01U);
}

/*!
    \brief      delay a time in milliseconds
    \param[in]  count: count in milliseconds
    \param[out] none
    \retval     none
*/
void delay_1ms(uint32_t count)
{
	delay = count;

	while(0U != delay) {
	}
}

/*!
    \brief      delay decrement
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay_decrement(void)
{
	if (0U != delay) {
		delay--;
	}
	ticks++;
}

/*!
    \brief      getTick
    \param[in]  none
    \param[out] none
    \retval     uint32_t ticsks
*/
uint32_t getTick(void)
{
	return ticks;
}
