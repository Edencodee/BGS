#include "main.h"
#include "ISL700_2A_38VC.H"


void IC_PinConfig(void);



int main(void)
{    
    systick_config();
    GPIO_UserInit();
	printf("Driver GPIO_BitBang.\r\n");
	// ISL700_2A简单测试
	printf("ISL700_2A Write Test Start...\r\n");
	//引脚配置
	IC_PinConfig();
	//寄存器配置
    //ISL_PLS = 255;           // 设置PLS值
    //ISL_Fine_Gain = 62;      // 设置Fine Gain
    //ISL_Coarse_Gain1 = 1;    // 设置Coarse Gain
    
    // 打印当前配置参数
    Print_ISL700_Config();
    
    WriteISL700_2A_38VC();     // 写入IC
    if(FuseCheck_flag == 0) {
        printf("Write Success!\r\n");
    } else {
        printf("Write Failed!\r\n");
    }

    while(1) {
        LED_Set(kLedYellow);
        delay_1ms(500);
        LED_Clr(kLedYellow); 
        delay_1ms(500);
    }
}


void IC_PinConfig(void)
{
    /* CLK_MODE 内部外部时钟选择  0:内部时钟, 1:外部时钟 */
    //CLKMode_Clr();  // 默认使用内部时钟
	CLKMode_Set();
    /* FREQ_CTRL 频率控制  0:低频 5KHz, 1:高频 16KHz */
//  FreqCtrl_Set(); // 默认高频
	FreqCtrl_Clr();
    /* CLOSE_PLS 关闭脉冲输出  0:开启脉冲输出, 1:关闭脉冲输出 */
    ClosePLS_Clr(); // 默认开启脉冲输出
	//ClosePLS_Set();
}

/* retarget the C library printf function to the RTT --------------- */
int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}
/* ----------------------------------------------------------------- */


