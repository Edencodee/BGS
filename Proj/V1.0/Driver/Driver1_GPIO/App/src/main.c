#include "main.h"


void BSP_ICPinConfig(void);

int main(void)
{    
    systick_config();
    GPIO_UserInit();
	BSP_ICPinConfig();
    printf("Driver_GPIO.\r\n");
	LED_Set(kLedGreen);
    while(1) {
		LED_Set(kLedYellow);
        delay_1ms(1000);
        LED_Clr(kLedYellow);
        delay_1ms(1000);
    }
}

void BSP_ICPinConfig(void)
{
    CLKMode_Clr();
    FreqCtrl_Set(); 
    ClosePLS_Clr(); 
}

/* retarget the C library printf function to the RTT --------------- */
int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}
/* ----------------------------------------------------------------- */