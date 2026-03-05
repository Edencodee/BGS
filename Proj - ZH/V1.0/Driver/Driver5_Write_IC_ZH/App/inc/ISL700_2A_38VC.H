#ifndef __ISL700_2A_38VC_H__
#define __ISL700_2A_38VC_H__

#include	"main.h"
#include	"CRC_8.H"

/* CAL control macros --------------------------------- */
#define CAL_IN_HIGH   			    gpio_bit_set(CAL_IN_PORT, CAL_IN_PIN)
#define CAL_IN_LOW   				gpio_bit_reset(CAL_IN_PORT, CAL_IN_PIN)
#define CAL_CLK_HIGH  			    gpio_bit_set(CAL_CLK_PORT, CAL_CLK_PIN)
#define CAL_CLK_LOW  			    gpio_bit_reset(CAL_CLK_PORT, CAL_CLK_PIN)
#define CRC_RST_HIGH    			gpio_bit_set(CRC_RST_PORT, CRC_RST_PIN)
#define CRC_RST_LOW     			gpio_bit_reset(CRC_RST_PORT, CRC_RST_PIN)
#define READ_STS                    gpio_input_bit_get(STS_PORT, STS_PIN)
/* ---------------------------------------------------- */


/**************************************
*	ISL700_2A内部增益设置寄存器的各项要修改的内容
***************************************/
extern unsigned char 	ISL_PLS;		   		//	4~11bit 	8bits	0~255
extern unsigned int    	ISL_Fine_Gain;		    //	14~23bit 	10bits	0~1023
extern unsigned char   	ISL_Coarse_Gain1;		//	29~32bit 	4bits	0~15

extern unsigned char   	ISL_CRC;				//	50~57bit	8bits 	CRC校验值

extern uint8_t FuseCheck_flag;	


void WriteISL700_2A_38VC(void);													// 写ISL700_2A程序
void Write_ISL700_2A_38VC_Uart(unsigned char *pdat, unsigned char Wei_Num);		//通过串口写ISL700_2A的数据
void Send_Byte(unsigned char length, unsigned int dat);							//单个数据写程序void Print_ISL700_Config(void);													//打印配置参数
void Print_ISL700_Config(void);
#endif
