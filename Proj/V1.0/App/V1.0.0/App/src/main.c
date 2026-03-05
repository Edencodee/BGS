#include "main.h"
#include "SEGGER_RTT.h"

#define APP_VERSION		"1.0.0"

__IO bool g_isSampleDone = true;
__IO bool g_isStartSampling = false;
__IO bool g_isSysTickInt = false;


volatile param_config_t g_param_cfg = PARAM_CONFIG_DEFAULT; 

int main(void)
{
    printf("App Version: %s\r\n", APP_VERSION);
	BSP_Init();
    Task_Init();
	ParamCfg_ApplyToIC();
    BSP_ICPinConfig();
	ParamCfg_Print(&g_param_cfg);
	delay_1ms(2000); 
    while (1) {
        //Test
		//exti_software_interrupt_enable(PLS_SO_EXTI_LINE);
		//delay_1ms(50);

//        if (g_isSysTickInt) {
//			g_isSysTickInt = false;
//			SysTickTask();
//		}

//        if(g_isStartSampling) {
//            g_isStartSampling = false;
//            MeasureTask();
//        } 

//        if(g_isSampleDone) {
//			g_isSampleDone = false;
//            AnalyzeTask();  
//            OutputTask();
//            LedTask();
//        }   
    }
}


