#include "main.h"
#include "SEGGER_RTT.h"

void BSP_Init(void)
{
    systick_config();
    GPIO_UserInit();
    PLS_SO_InterruptInit();  /* initialize external interrupt on PA6 (PLS_SO) */
    ADC_UserInit();          /* initialize ADC for regular mode */
}



int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}


