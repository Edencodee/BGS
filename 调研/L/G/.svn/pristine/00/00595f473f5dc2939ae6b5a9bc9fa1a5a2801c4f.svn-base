#include "main.h"


int main(void)
{
		GPIO_UserInit();
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));
    printf("\r\nCK_AHB is %d", rcu_clock_freq_get(CK_AHB));
    printf("\r\nCK_APB is %d", rcu_clock_freq_get(CK_APB));
/******************SET 1*********************/
//		PnpSet(kCh1);	//PA5
//	
//		LedSet((led_t)0);//PB3
//		LedSet((led_t)1);//PB4
//	
//		LDSet(kCh1);//PA4
//	
//		CRCRSTSet(kCh1);//PA5
//		CLKMODESet(kCh1);//PA7
//		CLOSEPLSMCUSet(kCh1);//PB0
//		FREQCTRLSet(kCh1);//PA15
/******************RESET 0*********************/	
		PnpClr(kCh1);	//PA5
	
		LedClr((led_t)0);//PB3
		LedClr((led_t)1);//PB4
	
		LDClr(kCh1);//PA4
	
		CRCRSTClr(kCh1);//PA5
		CLKMODEClr(kCh1);//PA7
		CLOSEPLSMCUClr(kCh1);//PB0
		FREQCTRLClr(kCh1);//PA15
	
    while(1) {
			
    }
}

/* retarget the C library printf function to the USART ------------- */
int fputc(int ch, FILE *f)
{
 SEGGER_RTT_PutChar(0, ch);
 return (ch);
}
/* ----------------------------------------------------------------- */