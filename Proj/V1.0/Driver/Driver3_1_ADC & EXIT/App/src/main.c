#include "main.h"

int main(void)
{    
    systick_config();
    GPIO_UserInit();
    PLS_SO_InterruptInit();  /* initialize external interrupt on PA6 (PLS_SO) */
    ADC_UserInit();           /* initialize ADC for regular mode */
    printf("Driver ADC & EXIT.\r\n");
	LED_Set(kLedGreen);
    while(1) {
        /* software trigger EXTI for ADC sampling */
        exti_software_interrupt_enable(PLS_SO_EXTI_LINE);
        delay_1ms(1000);
		
    }
}
/* retarget the C library printf function to the RTT --------------- */
int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}
/* ----------------------------------------------------------------- */