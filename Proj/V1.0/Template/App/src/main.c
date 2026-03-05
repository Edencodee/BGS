#include "main.h"

int main(void)
{    

    systick_config();
    GPIO_UserInit();
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    printf("CK_SYS is %d\n", rcu_clock_freq_get(CK_SYS));
    printf("CK_AHB is %d\n", rcu_clock_freq_get(CK_AHB));
    printf("CK_APB is %d\n", rcu_clock_freq_get(CK_APB));
    printf("Start\r\n");

    while(1) {
        LedSet(kLed1);
        delay_1ms(2000);
        LedClr(kLed1);
        delay_1ms(2000);
    }
}
/* retarget the C library printf function to the RTT --------------- */
int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}
/* ----------------------------------------------------------------- */