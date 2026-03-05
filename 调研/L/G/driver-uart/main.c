#include "main.h"


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 0);

    /* initialize the com */
    com_usart_init();

    /* enable USART TBE interrupt */
    usart_interrupt_enable(USART0, USART_INT_TBE);
		printf("test start!\r\n");
		WriteISL700_2A_38VC();
    while(1) {
    }
}

#define RTT 0
/* retarget the C library printf function to the USART ------------- */
#if RTT
int fputc(int ch, FILE *f)
{
 SEGGER_RTT_PutChar(0, ch);
 return (ch);
}
#else
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));

    return ch;
}
#endif
/* ----------------------------------------------------------------- */