#include "main.h"
#include "SEGGER_RTT.h"

void BSP_Init(void)
{
    systick_config();
    GPIO_UserInit();
    PLS_SO_InterruptInit();  /* initialize external interrupt on PA6 (PLS_SO) */
    ADC_UserInit();          /* initialize ADC for regular mode */
}

void ICPinConfig(void)
{
    /* CLK_MODE 内部外部时钟选择  0:内部时钟, 1:外部时钟 */
    CLKMode_Clr();  // 默认使用内部时钟
    /* FREQ_CTRL 频率控制  0:低频 5KHz, 1:高频 16KHz */
    FreqCtrl_Clr(); // 默认高频
	//FreqCtrl_Set(); 
    /* CLOSE_PLS 关闭脉冲输出  0:开启脉冲输出, 1:关闭脉冲输出 */
    ClosePLS_Clr(); // 默认开启脉冲输出
}

int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}


