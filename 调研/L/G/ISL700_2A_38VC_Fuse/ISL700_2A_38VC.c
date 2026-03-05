#include	"user_uart.h"
#include	"user_gpio.h"
#include	"ISL700_2A_38VC.h"

unsigned char CRC8_test[10] = {0};
unsigned char Bitcounter = 0;
unsigned char Bytecounter = 0;
unsigned char Datareg = 0;

/**************************************
*	ISL700_2A内部增益设置寄存器的各项要修改的内容
***************************************/
#define 				ISL_SR1				9			//	0~3bit 		4bits	固定值 9
unsigned char 	ISL_PLS				=255;		//	4~11bit 	8bits	0~255
#define					Folating			0			//	12~13bit 	4bits	0~3  
unsigned int 		ISL_Fine_Gain		=62;		//	14~23bit 	10bits	0~1023	
#define					ISL_Fine_Gainx1		1			//	24bit		1bit	0~1	
#define 				ISL_SR2				10			//	25~28bit 	4bits	固定值10
unsigned char 	ISL_Coarse_Gain1	=1;			//	29~32bit 	4bits	0~15
#define 				ISL_Coarse_Gain2 	0			//	33~36bit	4bits	0~15
#define					ISL_Sctrl_Res_a 	0			//	37~40bit 	4bits	0~15
#define					ISL_Sctrl_Res_b 	0			//	41~44bit 	4bits	0~15
#define					ISL_Sctrl_Res_Sel 	1			//	45bit 		1bits	0~1
#define 				ISL_SR3				6			//	46~49bit	4bits	固定值6
unsigned char   ISL_CRC				=0;			//	50~57bit	8bits 	CRC校验值
																	


bool FuseCheck_flag=0;	 //芯片输入数据校准标准位，为1时代表多次输入数据有误

/*******************************************************************
*函数名称：void CRC_table(unsigned char length,unsigned int dat)
*功能描述：组成字节
*全局变量：
*		 ：
*参数说明: length:数据长度；	dat：数据
*返回说明：一个字节数值
*说		明：
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
	CRC_table(10, ISL_Fine_Gain);					//	14~23bit 	10bits	0~1023
	CRC_table(1, ISL_Fine_Gainx1);					//	24bit		1bit	0~1
	CRC_table(4, ISL_SR2);							//	25~28bit 	4bits	固定值10
	CRC_table(4, ISL_Coarse_Gain1);					//	29~32bit 	4bits	0~15
	CRC_table(4, ISL_Coarse_Gain2);					//	33~36bit	4bits	0~15
	CRC_table(4, ISL_Sctrl_Res_a);					//	37~40bit 	4bits	0~15
	CRC_table(4, ISL_Sctrl_Res_b);					//	41~44bit 	4bits	0~15
	CRC_table(1, ISL_Sctrl_Res_Sel);				//	45bit 		1bits	0~1
	CRC_table(4, ISL_SR3);							//	46~49bit	4bits	固定值6
	ISL_CRC = cal_crc_table2(&CRC8_test, Bytecounter);
	printf("ISL_CRC:%.2X\r\n",ISL_CRC);
	Bytecounter = 0;	
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
	gpio_bit_reset(CRC_RST1_PORT,CRC_RST1_PIN);
	CRC_Table_Generate();
	gpio_bit_set(CRC_RST1_PORT,CRC_RST1_PIN);
	
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
	delay_us(10);
//	SetFlag(FUSEFLAG);
	while(!gpio_input_bit_get(STS_OUT1_PORT, STS_OUT1_PIN))
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
	gpio_bit_reset(CRC_RST1_PORT,CRC_RST1_PIN);
	delay_us(5);
	gpio_bit_set(CRC_RST1_PORT,CRC_RST1_PIN);
	for (i = Wei_Num; i > 0; i--)
	{
		Send_Byte(8, *pdat);
		pdat++;
	}
	while (!gpio_input_bit_get(STS_OUT1_PORT, STS_OUT1_PIN))
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
	  		gpio_bit_set(CAL_IN_PORT,CAL_IN_PIN);
	  	}
	  	else
	  	{
				gpio_bit_reset(CAL_IN_PORT,CAL_IN_PIN);
	  	}
		gpio_bit_set(Cal_CLK_PORT,Cal_CLK_PIN);
		gpio_bit_reset(Cal_CLK_PORT,Cal_CLK_PIN);
		dat = dat >> 1;	 //数据右移一位；	 
  }
}
