#include "main.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
//    fmc_erase_pages();

//    fmc_program();
   printf("check\r\n");
		SetFlag(kFlagOutState);
	int check=ChkFlag(kFlagOutState);
	printf("check:%d\r\n",check);
    while(1){
    }
}
/* retarget the C library printf function to the USART ------------- */
int fputc(int ch, FILE *f)
{
 SEGGER_RTT_PutChar(0, ch);
 return (ch);
}
/* ----------------------------------------------------------------- */