#include "main.h"
#include "systick.h"
#include "bsp_gpio.h"
#include "ISL700_2A_38VC.H"

static unsigned char CRC8_test[10] = {0};
static unsigned char Bitcounter = 0;
static unsigned char Bytecounter = 0;
static unsigned char Datareg = 0;

uint8_t FuseCheck_flag = 0; // 芯片输入数据校准标准位，为1时代表多次输入数据有误


/*******************************************************************
*函数名称：void CRC_table(unsigned char length,unsigned int dat)
*功能描述：组成字节
*参数说明: length:数据长度；	dat：数据
*返回说明：一个字节数值
******************************************************************/
static void CRC_table(unsigned char length, unsigned int dat)
{
	unsigned char wei;
	for (wei = 0; wei < length; wei++) {
		if (dat & 0x0001) {
			// 数据低位在前
			Datareg |= 0x80;
		} else {
			Datareg &= 0x7f;
		}
		if (Bitcounter++ == 7) {
			CRC8_test[Bytecounter++] = Datareg;
			Datareg = 0;
			Bitcounter = 0;
		}
		Datareg >>= 1;
		dat >>= 1;
	}
}

static uint8_t ISL700_CalcCrc(volatile isl700_config_t *cfg)
{
	if (cfg == NULL) {
		return 0;
	}

	//CRC Cal Reset
	Bitcounter = 0;
	Bytecounter = 0;
	Datareg = 0;
	
	CRC_table(6, cfg->fuse_check_floating);         // 不够8bit整数倍需在前端补0，个数为6
	CRC_table(4, cfg->fuse_check_low);              // 0~3bit, 4bits, 固定值9
	CRC_table(8, cfg->pls);                          // 4~11bit, 8bits
	CRC_table(2, cfg->no_used);                      // 12~13bit, 2bits
	CRC_table(10, cfg->fine_gain);                   // 14~23bit, 10bits
	CRC_table(1, cfg->gain_select);                  // 24bit, 1bit
	CRC_table(4, cfg->fuse_check_middle);            // 25~28bit, 4bits, 固定值10
	CRC_table(4, cfg->coarse_gain1);                 // 29~32bit, 4bits
	CRC_table(4, cfg->coarse_gain2);                 // 33~36bit, 4bits
	CRC_table(4, cfg->sctrl_res_a);                  // 37~40bit, 4bits
	CRC_table(4, cfg->sctrl_res_b);                  // 41~44bit, 4bits
	CRC_table(1, cfg->sctrl_internal);               // 45bit, 1bit
	CRC_table(4, cfg->fuse_check_high);              // 46~49bit, 4bits, 固定值6
	
	return cal_crc_table(CRC8_test, Bytecounter);
}

/******************************************************************** 
* 名称: void Send_Byte(unsigned char length, unsigned int dat) 
* 功能: 单个数据写程序
* 调用: 
* 输入:  length: 数据的长度。dat:输入的数据。
* 返回值: 
***********************************************************************/
static void Send_Byte(unsigned char length, unsigned int dat)
{
	unsigned char wei;
	for (wei = 0; wei < length; wei++) {
		if ((dat & 0x0001) == 1) {
			// MCU通过Cal_IN，Cal_CLK向芯片传输数据
			CAL_IN_HIGH;
		} else {
			CAL_IN_LOW;
		}
		CAL_CLK_HIGH;
		CAL_CLK_LOW;
		dat = dat >> 1; // 数据右移一位
	}
}


/******************************************************************** 
* 名称: void IC_WriteReg(const volatile isl700_config_t *cfg)
* 功能: 写ISL700_2A_38VC程序
* 调用: 
* 输入: 
* 返回值: 
***********************************************************************/
static void IC_WriteReg(volatile isl700_config_t *cfg)
{
	unsigned char Write_counter = 0;
	uint8_t crc8 = 0;

	if (cfg == NULL) {
		FuseCheck_flag = 1;
		return;
	}

	FuseCheck_flag = 0;
Table:
	CRC_RST_LOW;
	cfg->crc8 = ISL700_CalcCrc(cfg);
	CRC_RST_HIGH;
	
	Send_Byte(6, cfg->fuse_check_floating); // 不够8bit整数倍需在前端补0，个数为6
	Send_Byte(4, cfg->fuse_check_low);      // 0~3bit, 4bits, 固定值9
	Send_Byte(8, cfg->pls);                 // 4~11bit, 8bits
	Send_Byte(2, cfg->no_used);             // 12~13bit, 2bits
	Send_Byte(10, cfg->fine_gain);          // 14~23bit, 10bits
	Send_Byte(1, cfg->gain_select);         // 24bit, 1bit
	Send_Byte(4, cfg->fuse_check_middle);   // 25~28bit, 4bits, 固定值10
	Send_Byte(4, cfg->coarse_gain1);        // 29~32bit, 4bits
	Send_Byte(4, cfg->coarse_gain2);        // 33~36bit, 4bits
	Send_Byte(4, cfg->sctrl_res_a);         // 37~40bit, 4bits
	Send_Byte(4, cfg->sctrl_res_b);         // 41~44bit, 4bits
	Send_Byte(1, cfg->sctrl_internal);      // 45bit, 1bit
	Send_Byte(4, cfg->fuse_check_high);     // 46~49bit, 4bits, 固定值6
	Send_Byte(8, cfg->crc8);                // 50~57bit, 8bits, CRC
	
	
	delay_1us(10);
	while (!READ_STS) {
		if (++Write_counter > 3) {
			FuseCheck_flag = 1;
			break;
		}
		goto Table;
	}
}

/******************************************************************** 
* 名称: void ISL700_WriteConfig(const volatile isl700_config_t *cfg)
* 功能: 写ISL700_2A_38VC程序
* 调用: 
* 输入: 
* 返回值: 
***********************************************************************/
void ISL700_WriteConfig(volatile isl700_config_t *cfg)
{
	IC_WriteReg(cfg);
}

/******************************************************************** 
* 名称: void ISL700_PrintfConfig(const volatile isl700_config_t *cfg)
* 功能: 打印ISL700_2A所有配置参数
* 调用: 
* 输入: 无
* 返回值: 无
***********************************************************************/
void ISL700_PrintfConfig(const volatile isl700_config_t *cfg)
{
	if (cfg == NULL) {
		return;
	}

	printf("\r\n======= ISL700_2A Configuration =======\r\n");
	printf("  FUSE_L         : %d (0~3bit, 4bits)\r\n", cfg->fuse_check_low);
	printf("  PLS            : %d (4~11bit, 8bits, 0~255)   ***\r\n", cfg->pls);
	printf("  NO_USED        : %d (12~13bit, 2bits)\r\n", cfg->no_used);
	printf("  FINE_GAIN      : %d (14~23bit, 10bits, 0~1023) ***\r\n", cfg->fine_gain);
	printf("  GAIN_SELECT    : %d (24bit, 1bit)\r\n", cfg->gain_select);
	printf("  FUSE_M         : %d (25~28bit, 4bits)\r\n", cfg->fuse_check_middle);
	printf("  COARSE_GAIN1   : %d (29~32bit, 4bits, 0~15)     ***\r\n", cfg->coarse_gain1);
	printf("  COARSE_GAIN2   : %d (33~36bit, 4bits)\r\n", cfg->coarse_gain2);
	printf("  SCTRL_RES_A    : %d (37~40bit, 4bits)\r\n", cfg->sctrl_res_a);
	printf("  SCTRL_RES_B    : %d (41~44bit, 4bits)\r\n", cfg->sctrl_res_b);
	printf("  SCTRL_INTERNAL : %d (45bit, 1bit)\r\n", cfg->sctrl_internal);
	printf("  FUSE_H         : %d (46~49bit, 4bits)\r\n", cfg->fuse_check_high);
	printf("  CRC8           : %d (50~57bit, 8bits, CRC)      ***\r\n", cfg->crc8);
	printf("=======================================\r\n\r\n");
}

void IC_PinConfig(void)
{
	/* Select internal or external clock source for ISL700_2A_38VC */
    //CLKMode_Clr();   /* Enable internal clock (default) */ 
	CLKMode_Set();    /* Enable external clock */


    /* FREQ_CTRL 频率控制  1:低频 5KHz, 0:高频 16KHz */
	//FreqCtrl_Clr();   /* High frequency 16KHz */
	FreqCtrl_Set();     /* Low frequency 5KHz (default) */

    /* CLOSE_PLS 关闭脉冲输出  0:开启脉冲输出, 1:关闭脉冲输出 */
    ClosePLS_Clr();   /* Enable pulse output */
	//ClosePLS_Set();   /* Disable pulse output */
}

void IC_ExternalClkInit(void)
{
	/* configure MCO GPIO port */
	rcu_osci_on(RCU_IRC48M);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC48M)) {
        while(1) {
			printf("RCU_IRC48M Error\n");
			delay_1ms(1000);
        }
    }
	rcu_osci_on(RCU_IRC32K);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC32K)) {
        while(1) {
			printf("RCU_IRC32K Error\n");
			delay_1ms(1000);
        }
    }
    rcu_periph_clock_enable(EXT_CLK_CLK);
	//HS -> PA9
	gpio_mode_set(EXT_CLK_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EXT_HS_CLK_PIN);
	gpio_af_set(EXT_CLK_PORT, GPIO_AF_0, EXT_HS_CLK_PIN);
	rcu_ckout0_config(RCU_CKOUT0SRC_IRC48M, RCU_CKOUT0_DIV32);//6MHz 
	//LS -> PA10
	gpio_mode_set(EXT_CLK_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EXT_LS_CLK_PIN);
	gpio_af_set(EXT_CLK_PORT, GPIO_AF_3, EXT_LS_CLK_PIN);
	rcu_ckout1_config(RCU_CKOUT1SRC_IRC32K,RCU_CKOUT1_DIV8);//4KHz
}

void ISL700_Init(volatile isl700_config_t *cfg)
{
    /* Initialize external clock if needed */
    //IC_ExternalClkInit();

    /*Write configuration to ISL700*/
    ISL700_WriteConfig(cfg);
	// Initialize GPIO pins
    IC_PinConfig();
	
	if(FuseCheck_flag == 0) {
        printf("Write Success!\r\n");
		/* Printf configuration */
    	//ISL700_PrintfConfig(cfg);
    } else {
        printf("Write Failed!\r\n");
    }
}