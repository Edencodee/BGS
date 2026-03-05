#include "bsp_gpio.h"

/* IO array -------------------------------------------------------------------------------- */

const uint32_t LED_PORT[LED_N] = {LED1_PORT};
const uint32_t LED_PIN[LED_N] = {LED1_PIN};
const rcu_periph_enum LED_CLK[LED_N] = {LED1_CLK};

/* ----------------------------------------------------------------------------------------- */


/* led for test -------------------------------------------------------------------- */
void LedSet (led_t led)
{
	GPIO_BOP(LED_PORT[led]) = LED_PIN[led];
}

void LedClr (led_t led)
{
	GPIO_BC(LED_PORT[led]) = LED_PIN[led];
}
/* ----------------------------------------------------------------------------------------- */


void GPIO_UserInit(void)
{
	/* configure LED GPIO port */
	for (int i = 0; i < LED_N; i++) {
		rcu_periph_clock_enable(LED_CLK[i]);
		gpio_mode_set(LED_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN[i]);
		GPIO_BC(LED_PORT[i]) = LED_PIN[i];
	}
}
