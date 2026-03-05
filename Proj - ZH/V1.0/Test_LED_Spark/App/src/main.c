#include "main.h"


void led_spark(void)
{
  
}

int main(void)
{    
    systick_config();
    /* enable the LED1 GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* configure LED1 GPIO port */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, GPIO_PIN_8);

    /* reset LED1 GPIO pin */
    gpio_bit_reset(GPIOB, GPIO_PIN_8);

    while(1) {
        /* turn on LED1 */
        gpio_bit_set(GPIOB, GPIO_PIN_8);
        delay_1ms(500);
        /* turn off LED1 */
        gpio_bit_reset(GPIOB, GPIO_PIN_8);
        delay_1ms(500);
    }

}
