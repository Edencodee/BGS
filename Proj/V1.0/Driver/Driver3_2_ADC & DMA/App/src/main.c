#include "main.h"

int main(void)
{    
    systick_config();
    GPIO_UserInit();
    PLS_SO_InterruptInit();  /* initialize external interrupt on PA6 (PLS_SO) */
    ADC_UserInit();           /* initialize ADC for regular mode */
    printf("Driver ADC & DMA & EXIT.\r\n");
	LED_Set(kLedGreen);
    exti_software_interrupt_enable(PLS_SO_EXTI_LINE);
    //delay_1ms(10);
    while(1) {
		exti_software_interrupt_enable(PLS_SO_EXTI_LINE);
		delay_1ms(1000);
        if(g_isSampleDone) {
            g_isSampleDone = 0;
            printf("ADC Values:\r\n");
            for(uint8_t i = 0; i < ADC_BUF_LEN; i++) {
                printf("%d\r\n", g_adcCaptureBuffer[i]);
            }
        }
    }
}

/* retarget the C library printf function to the RTT --------------- */
int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}
/* ----------------------------------------------------------------- */