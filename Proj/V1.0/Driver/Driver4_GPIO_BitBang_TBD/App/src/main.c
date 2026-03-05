#include "main.h"


void IC_PinConfig(void);

// ISL700_2A config sequence:
// 1. Pulse CRC_RST low to reset internal CRC checker
// 2. Send 58-bit shift register data via CAL_IN/CAL_CLK (MSB first)
// 3. Check STS pin for CRC/fixed-bit verification

int main(void)
{    
    systick_config();
    GPIO_UserInit();
	//引脚配置
	IC_PinConfig();
	//寄存器配置
    printf("Driver GPIO_BitBang.\r\n");
    /* 可选：修改部分参数 */
//    g_isl700_reg.pls = 200;
//    g_isl700_reg.fine_gain = 80;
//    g_isl700_reg.coarse_gain1 = 2;
	
    bool ok = ISL700_Write_Reg(&g_isl700_reg);
    if (ok)
    {
        printf("ISL700_2A configured successfully.\r\n");
    }
    else
    {
        printf("ISL700_2A configuration failed.\r\n");
    }

    while(1) {
//        LED_Set(kLedYellow);
//        delay_1ms(500);
//        LED_Clr(kLedYellow); 
//        delay_1ms(500);
    }
}


void IC_PinConfig(void)
{
    /* CLK_MODE 内部外部时钟选择  0:内部时钟, 1:外部时钟 */
    CLKMode_Clr();  // 默认使用内部时钟
    /* FREQ_CTRL 频率控制  0:低频 5KHz, 1:高频 16KHz */
    FreqCtrl_Set(); // 默认高频
    /* CLOSE_PLS 关闭脉冲输出  0:开启脉冲输出, 1:关闭脉冲输出 */
    ClosePLS_Clr(); // 默认开启脉冲输出
}

/* retarget the C library printf function to the RTT --------------- */
int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}
/* ----------------------------------------------------------------- */


