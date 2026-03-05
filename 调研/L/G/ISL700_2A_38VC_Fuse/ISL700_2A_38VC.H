#ifndef __ISL700_2A_38VC_H__
#define __ISL700_2A_38VC_H__

#include	"user_uart.h"
#include	"CRC_8.h"
#include "systick.h"
#include "user_uart.h"

/**************************************
*	ISL700_2A内部增益设置寄存器的各项要修改的内容
***************************************/
extern unsigned char 	ISL_PLS;		   		//	4~11bit 	8bits	0~255
extern unsigned int   ISL_Fine_Gain;		    //	14~23bit 	10bits	0~1023
extern unsigned char  ISL_Coarse_Gain1;		//	29~32bit 	4bits	0~15

extern unsigned char  ISL_CRC;				//	50~57bit	8bits 	CRC校验值

extern bool FuseCheck_flag;	


void WriteISL700_2A_38VC(void);													// 写ISL700_2A程序
void Write_ISL700_2A_38VC_Uart(unsigned char *pdat, unsigned char Wei_Num);		//通过串口写ISL700_2A的数据
void Send_Byte(unsigned char length, unsigned int dat);							//单个数据写程序

#define crc_rst
#define STS
#define Cal_IN	gpio_input_bit_get(STS_OUT1_PORT, STS_OUT1_PIN)
#define Cal_CLK
#endif
