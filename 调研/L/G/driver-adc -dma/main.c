#include "main.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();

    /* configure RCU peripheral */
		ADC_UserInit();

    printf("\r /**** ADC Demo ****/\r\n");

    while(1){
        delay_1ms(1000);
        printf("\r\n *******************");
        printf("\r\n ADC: PA1, adc_value = %04X",adc_value);
        printf("\r\n ******************* \r\n");
    }
}

/* retarget the C library printf function to the USART ------------- */
int fputc(int ch, FILE *f)
{
 SEGGER_RTT_PutChar(0, ch);
 return (ch);
}
/* ----------------------------------------------------------------- */