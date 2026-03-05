#include "main.h"
#include "systick.h"
#include "bsp_gpio.h"
#include "ISL700_2A_38VC.H"

unsigned char CRC8_test[10] = {0};
unsigned char Bitcounter = 0;
unsigned char Bytecounter = 0;
unsigned char Datareg = 0;

/**************************************
* ISL700_2A内部增益设置寄存器的各项要修改的内容
***************************************/
unsigned char 			ISL_SR1				=9;			//	0~3bit 		4bits	固定值 9
unsigned  char 	        ISL_PLS				=255;		//	4~11bit 	8bits	0~255     ***
unsigned char			Folating			=1;			//	12~13bit 	4bits	0~3  
unsigned int  	        ISL_Fine_Gain		=62;		//	14~23bit 	10bits	0~1023	  ***
unsigned char 			ISL_Fine_Gainx1		=0;			//	24bit		1bit	0~1	
unsigned char 			ISL_SR2				=10;			//	25~28bit 	4bits	固定值10 
unsigned  char 	        ISL_Coarse_Gain1	=15;			//	29~32bit 	4bits	0~15      ***
unsigned char 			ISL_Coarse_Gain2 	=1;			//	33~36bit	4bits	0~15
unsigned char			ISL_Sctrl_Res_a 	=5;			//	37~40bit 	4bits	0~15
unsigned char			ISL_Sctrl_Res_b 	=5;			//	41~44bit 	4bits	0~15
unsigned char			ISL_Sctrl_Res_Sel 	=1;			//	45bit 		1bits	0~1
unsigned char 			ISL_SR3				=6;			//	46~49bit	4bits	固定值6
unsigned char   	    ISL_CRC				=0;		    //	50~57bit	8bits 	CRC校验值  ***
																	

//#define 				ISL_SR1				9			//	0~3bit 		4bits	固定值 9
//unsigned char 	        ISL_PLS				=255;		//	4~11bit 	8bits	0~255     ***
//#define					Folating			1			//	12~13bit 	4bits	0~3  
//unsigned int 	        ISL_Fine_Gain		=62;		//	14~23bit 	10bits	0~1023	  ***
//#define 				ISL_Fine_Gainx1		0			//	24bit		1bit	0~1	
//#define 				ISL_SR2				10			//	25~28bit 	4bits	固定值10 
//unsigned char 	        ISL_Coarse_Gain1	=15;			//	29~32bit 	4bits	0~15      ***
//#define 				ISL_Coarse_Gain2 	1			//	33~36bit	4bits	0~15
//#define					ISL_Sctrl_Res_a 	5			//	37~40bit 	4bits	0~15
//#define					ISL_Sctrl_Res_b 	5			//	41~44bit 	4bits	0~15
//#define					ISL_Sctrl_Res_Sel 	1			//	45bit 		1bits	0~1
//#define 				ISL_SR3				6			//	46~49bit	4bits	固定值6
//unsigned char   	    ISL_CRC				=0;		    //	50~57bit	8bits 	CRC校验值  ***
//																	


uint8_t FuseCheck_flag=0;	 //芯片输入数据校准标准位，为1时代表多次输入数据有误

/*******************************************************************
*函数名称：void CRC_table(unsigned char length,unsigned int dat)
*功能描述：组成字节
*参数说明: length:数据长度；	dat：数据
*返回说明：一个字节数值
******************************************************************/
void CRC_table(unsigned char length, unsigned int dat)
{
	unsigned char wei;
	for (wei = 0; wei < length; wei++)
	{
		if(dat & 0x0001)	  //数据低位在前
		{
			Datareg |= 0x80;	
		}
		else
		{
			Datareg &= 0x7f;
		}
		if(Bitcounter++ == 7)
		{
			CRC8_test[Bytecounter++] = Datareg;  
			Datareg = 0;
			Bitcounter = 0;
		}
		Datareg >>= 1;
		dat >>= 1;	
	}
}

/*******************************************************************
*函数名称：void CRC_Table_Generate()
*功能描述：调用组成字节函数产生8位CRC校验码
*全局变量：
*		 ：
*参数说明: 无
*返回说明：无
*说		明：
******************************************************************/
void CRC_Table_Generate(void)
{
	CRC_table(6, 0);								//	不够8bit整数倍需在前端补0，个数为6		
	CRC_table(4, ISL_SR1);							//	0~3bit 		4bits	固定值 9
	CRC_table(8, ISL_PLS);							//	4~11bit 	8bits	0~255
	CRC_table(2, Folating);							//	12~13bit 	2bits	0~3  
	CRC_table(10,ISL_Fine_Gain);					//	14~23bit 	10bits	0~1023
	CRC_table(1, ISL_Fine_Gainx1);					//	24bit		1bit	0~1
	CRC_table(4, ISL_SR2);							//	25~28bit 	4bits	固定值10
	CRC_table(4, ISL_Coarse_Gain1);					//	29~32bit 	4bits	0~15
	CRC_table(4, ISL_Coarse_Gain2);					//	33~36bit	4bits	0~15
	CRC_table(4, ISL_Sctrl_Res_a);					//	37~40bit 	4bits	0~15
	CRC_table(4, ISL_Sctrl_Res_b);					//	41~44bit 	4bits	0~15
	CRC_table(1, ISL_Sctrl_Res_Sel);				//	45bit 		1bits	0~1
	CRC_table(4, ISL_SR3);							//	46~49bit	4bits	固定值6
	ISL_CRC = cal_crc_table2(CRC8_test, Bytecounter);
	Bytecounter = 0;
	printf("CRC: %d\r\n",ISL_CRC);	
}

/******************************************************************** 
* 名称: void WriteISL700_2A_38VC(void)
* 功能: 写ISL700_2A_38VC程序
* 调用: 
* 输入: 
* 返回值: 
***********************************************************************/
void WriteISL700_2A_38VC(void)
{	
	unsigned char Write_counter=0;

	FuseCheck_flag = 0;
Table:	 
	CRC_RST_LOW;
	CRC_Table_Generate();
	CRC_RST_HIGH;
	
	Send_Byte(6, 0);		 				//不够8bit整数倍需在前端补0，个数为6
	Send_Byte(4, ISL_SR1);					//	0~3bit 		4bits	固定值 9
	Send_Byte(8, ISL_PLS);					//	4~11bit 	8bits	0~255
	Send_Byte(2, Folating);					//	12~13bit 	2bits	0~3  
	Send_Byte(10, ISL_Fine_Gain);			//	14~23bit 	10bits	0~1023
	Send_Byte(1, ISL_Fine_Gainx1);			//	24bit		1bit	0~1
	Send_Byte(4, ISL_SR2);					//	25~28bit 	4bits	固定值10
	Send_Byte(4, ISL_Coarse_Gain1);			//	29~32bit 	4bits	0~15
	Send_Byte(4, ISL_Coarse_Gain2);			//	33~36bit	4bits	0~15
	Send_Byte(4, ISL_Sctrl_Res_a);			//	37~40bit 	4bits	0~15
	Send_Byte(4, ISL_Sctrl_Res_b);			//	41~44bit 	4bits	0~15
	Send_Byte(1, ISL_Sctrl_Res_Sel);		//	45bit 		1bits	0~1
	Send_Byte(4, ISL_SR3);					//	46~49bit	4bits	固定值6
	Send_Byte(8, ISL_CRC);					//	50~57bit	8bits 	CRC校验值
	delay_1us(10);
	while(!READ_STS)
	{
		if (++Write_counter > 3)
		{
		    FuseCheck_flag = 1;
			break;
		}
		goto Table;	
	}
}

/******************************************************************** 
* 名称: void Write_ISL700_2A_38VC_Uart(unsigned char *pdat, unsigned char Wei_Num)
* 功能: 
* 调用: 
* 输入: 
* 返回值: 
* 说明： 通过串口写ISL700_2A的数据
***********************************************************************/
void Write_ISL700_2A_38VC_Uart(unsigned char *pdat, unsigned char Wei_Num)
{
	unsigned char i;
	unsigned char Write_counter1 = 0;
	FuseCheck_flag = 0; 

mark2:
	CRC_RST_LOW;
	delay_1us(5);
	CRC_RST_HIGH;
	for (i = Wei_Num; i > 0; i--)
	{
		Send_Byte(8, *pdat);
		pdat++;
	}
	while (!READ_STS)
	{
		if (++Write_counter1 > 3)
		{
		    FuseCheck_flag = 1;
			break;
		}
		goto mark2;	
	}
}

/******************************************************************** 
* 名称: void Send_Byte(unsigned char length, unsigned int dat) 
* 功能: 单个数据写程序
* 调用: 
* 输入:  length: 数据的长度。dat:输入的数据。
* 返回值: 
***********************************************************************/
void Send_Byte(unsigned char length, unsigned int dat) 
{
	unsigned char wei;
  	for (wei = 0; wei < length; wei++)
  	{
  	  	if ((dat & 0x0001) == 1)	//MCU通过Cal_IN，Cal_CLK向芯片传输数据
	  	{
	  		CAL_IN_HIGH;
	  	}
	  	else
	  	{
	  		CAL_IN_LOW;
	  	}
		CAL_CLK_HIGH; 
		CAL_CLK_LOW;
		dat = dat >> 1;	 //数据右移一位；	 
  }
}

/******************************************************************** 
* 名称: void Print_ISL700_Config(void)
* 功能: 打印ISL700_2A所有配置参数
* 调用: 
* 输入: 无
* 返回值: 无
***********************************************************************/
void Print_ISL700_Config(void)
{
	printf("\r\n======= ISL700_2A Configuration =======\r\n");
	printf("  ISL_SR1          : %d (0~3bit, 4bits)\r\n", ISL_SR1);
	printf("  ISL_PLS          : %d (4~11bit, 8bits, 0~255)   ***\r\n", ISL_PLS);
	printf("  Folating         : %d (12~13bit, 2bits)\r\n", Folating);
	printf("  ISL_Fine_Gain    : %d (14~23bit, 10bits, 0~1023) ***\r\n", ISL_Fine_Gain);
	printf("  ISL_Fine_Gainx1  : %d (24bit, 1bit)\r\n", ISL_Fine_Gainx1);
	printf("  ISL_SR2          : %d (25~28bit, 4bits)\r\n", ISL_SR2);
	printf("  ISL_Coarse_Gain1 : %d (29~32bit, 4bits, 0~15)     ***\r\n", ISL_Coarse_Gain1);
	printf("  ISL_Coarse_Gain2 : %d (33~36bit, 4bits)\r\n", ISL_Coarse_Gain2);
	printf("  ISL_Sctrl_Res_a  : %d (37~40bit, 4bits)\r\n", ISL_Sctrl_Res_a);
	printf("  ISL_Sctrl_Res_b  : %d (41~44bit, 4bits)\r\n", ISL_Sctrl_Res_b);
	printf("  ISL_Sctrl_Res_Sel: %d (45bit, 1bit)\r\n", ISL_Sctrl_Res_Sel);
	printf("  ISL_SR3          : %d (46~49bit, 4bits)\r\n", ISL_SR3);
	printf("  ISL_CRC          : 0x%02X (50~57bit, 8bits, CRC)   ***\r\n", ISL_CRC);
	printf("=======================================\r\n\r\n");
}

