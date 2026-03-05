#include "main.h"

extern uint16_t adc_value;

void Pd_Init(void)
{
		int a=1;
		/********PDtask Init*******/
		//timer operation			clk_mode  freq-c Close_pls
		CLKMODEClr(1);			//CLKMODESet
		FREQCTRLClr(1);
		CLOSEPLSMCUClr(1);
			
		//Enable clock output  ext-slvclk,ext-clk-hf
		EXTLS();
		EXTHS();
		//CRC init	CRC_rst
		CRCRSTClr(1);
		/********PDtask setting*******/
		//SR Register Settings	cal-clk,cal-in
		WriteISL700_2A_38VC();
		//CRC judge		STS
		if(a==1){
			CRCRSTClr(1);
			WriteISL700_2A_38VC();
			delay_1ms(10);
		if(GetSTS(1)==SET){
			printf("output signal correct!\r\n");
			a=0;
		}
		else{
			printf("output signal error!\r\n");
			a=1;
		}
	}
}

void PDTask (void)
{
	int ch0;
	int ch1;
	//Judge flag output Pls_s0
	//ADC OUTPUT		SH_OUT
	if(PLSSO(1)==SET){
		ch0=adc_value;
	}
	else{
		ch1=adc_value;
	}
}
