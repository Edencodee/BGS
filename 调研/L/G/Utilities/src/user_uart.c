#include "user_uart.h"

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE            (ARRAYNUM(transmitter_buffer) - 1)

uint8_t transmitter_buffer[] = "\n\rUSART interrupt test\n\r";
uint8_t receiver_buffer[32];
uint8_t transfersize = TRANSMIT_SIZE;
uint8_t receivesize = 32;
__IO uint8_t txcount = 0;
__IO uint16_t rxcount = 0;


/*!
    \brief      initialize the USART configuration of the com
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_usart_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(CAL_IN_CLK);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USART TX */
    gpio_af_set(CAL_IN_PORT, CAL_IN_AF, CAL_IN_PIN);

    /* configure USART TX as alternate function push-pull */
    gpio_mode_set(CAL_IN_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, CAL_IN_PIN);
    gpio_output_options_set(CAL_IN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, CAL_IN_PIN);

    /* USART configure */
    usart_deinit(USART0);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    usart_enable(USART0);
}

void UART_SendData (uint32_t usartPeriph, uint8_t* buf, uint32_t size)
{
	for (int i = 0; i < size; i++) {
		usart_data_transmit(usartPeriph, buf[i]);
		while (RESET == usart_flag_get(usartPeriph, USART_FLAG_TBE)) {
		}
	}
}