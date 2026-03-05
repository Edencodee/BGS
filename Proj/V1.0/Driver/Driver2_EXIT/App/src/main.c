#include "main.h"


int main(void)
{    
	printf("Driver_Exit.\r\n");
	systick_config();
    GPIO_UserInit();
    PLS_SO_InterruptInit();
    
	LED_Set(kLedGreen);
    while(1) {
		LED_Set(kLedYellow);
		delay_1ms(1000);
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