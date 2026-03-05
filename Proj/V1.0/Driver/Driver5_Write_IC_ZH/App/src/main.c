#include "main.h"
#include "ISL700_2A_38VC.H"


void IC_PinConfig(void);
void IC_ClockConfig(void);


int main(void)
{    
    systick_config();
	rcu_periph_clock_enable(RCU_SYSCFG);
	syscfg_pin_remap_enable(SYSCFG_CFG0_PA11_RMP);
	syscfg_pin_remap_enable(SYSCFG_CFG0_PA12_RMP);
    GPIO_UserInit();
	printf("Driver GPIO_BitBang.\r\n");
	// ISL700_2A简单测试
	printf("ISL700_2A Write Test Start...\r\n");
	//引脚配置
	IC_PinConfig();
	IC_ClockConfig();
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
    //CLKMode_Clr();    // 使能内部时钟
	CLKMode_Set();    //使能外部时钟

    /* FREQ_CTRL 频率控制  1:低频 5KHz, 0:高频 16KHz */
    //FreqCtrl_Set();   // 低频输出
	FreqCtrl_Clr();   // 高频输出

    /* CLOSE_PLS 关闭脉冲输出  0:开启脉冲输出, 1:关闭脉冲输出 */
    ClosePLS_Clr();   // 使能脉冲输出
	//ClosePLS_Set();   // 关闭脉冲输出
}

void IC_ClockConfig(void)
{
    /* configure MCO GPIO port */
	rcu_periph_clock_enable(EXT_CLK_CLK);
	rcu_osci_on(RCU_IRC32K);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC32K)) {
        while(1) {
			printf("Error\n");
			delay_1ms(1000);
        }
    }
	//HS -> PA9
	gpio_mode_set(EXT_CLK_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EXT_HS_CLK_PIN);
	gpio_af_set(EXT_CLK_PORT, GPIO_AF_0, EXT_HS_CLK_PIN);
	rcu_ckout0_config(RCU_CKOUT0SRC_IRC48M, RCU_CKOUT0_DIV8);
	//LS -> PA10
	gpio_mode_set(EXT_CLK_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EXT_LS_CLK_PIN);
	gpio_af_set(EXT_CLK_PORT, GPIO_AF_3, EXT_LS_CLK_PIN);
	rcu_ckout1_config(RCU_CKOUT1SRC_IRC32K,RCU_CKOUT1_DIV2);

}

/* retarget the C library printf function to the RTT --------------- */
int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}
/* ----------------------------------------------------------------- */


