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
	//ParamCfg_ApplyToIC();
    ICPinConfig();
	//ParamCfg_Print(&g_param_cfg);
    
	start_timing_ms();
	delay_1ms(1000); 
    print_timing_ms(stop_timing_ms());
	
	Print_ISL700_Config();
    WriteISL700_2A_38VC();     
    if(FuseCheck_flag == 0) {
        printf("Write Success!\r\n");
    } else {
        printf("Write Failed!\r\n");
    }
	
    while (1) {
        if (g_isSysTickInt) {
			g_isSysTickInt = false;
			//SysTickTask();
		}

        if(g_isStartSampling) {
            g_isStartSampling = false;
            MeasureTask();
        } 

        if(g_isSampleDone) {
			g_isSampleDone = false;
            AnalyzeTask();  
            OutputTask();
            LedTask();
        }
    }
}


