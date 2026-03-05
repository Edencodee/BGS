#include "main.h"
#include "SEGGER_RTT.h"

#define APP_VERSION		"1.0.2"

__IO bool g_isSampleDone = true;
__IO bool g_isStartSampling = false;
__IO bool g_isSysTickInt = false;
__IO bool g_isA_Done = false;   /* A channel DMA sampling complete flag */
__IO bool g_isB_Done = false;   /* B channel DMA sampling complete flag */

__IO bool g_pauseMainLoop = false; /* Set to true to exit main loop and pause program */

/* ==================== Global Variables ==================== */
//param_config_t g_param_cfg;/* Current parameters (RAM), managed by task_flash module (loaded by Param_Init()) */
param_config_t g_param_cfg;  // Will be initialized from g_IC_user_config

int main(void)
{
    uint16_t debug_group_count = 0U;

    printf("App Version: %s\r\n", APP_VERSION);
    
    /* Initialize g_param_cfg with default user config */
    memcpy(&g_param_cfg, &g_IC_user_config, sizeof(param_config_t));
    
	BSP_Init();
    Task_Init();
    
#if DEBUG_USE_DEFAULT_CONFIG
    /* Debug mode: Force default config (ignore Flash) */
    printf("[DEBUG] Force using default config\r\n");
    Param_ForceDefault();
#endif
    
    /* Flash parameter test (uncomment to test) */
    //Test_FlashParam();
    delay_1ms(50);
    ISL700_Init(&g_param_cfg.isl700);
	ISL700_PrintfConfig(&g_param_cfg.isl700);
	delay_1ms(500);

    while (1) {
        if (g_pauseMainLoop) {
            break;
        }

        if (g_isSysTickInt) {
			g_isSysTickInt = false;
			//Debug
			continue;
			SysTickTask();
		}

        if(g_isStartSampling) {
            MeasureTask();
			g_isStartSampling = false;
        } 

        /* Only when both A and B channels have completed sampling, a complete measurement is formed */
        if(g_isA_Done && g_isB_Done) {
            g_isA_Done = false;
            g_isB_Done = false;
            AnalyzeTask();		
            /* Only when the average samples are sufficient, the output task is executed */
            if(g_isVoutReady) {
                g_isVoutReady = false;
				
				//Debug
				continue;
#if DEBUG_MODE_ENABLE
                /* Debug mode: Periodically print and pause */
                if (debug_group_count < DEBUG_CAPTURE_GROUPS) {
                    debug_group_count++;
                    printf("=== Debug Group %d ===\r\n", debug_group_count);
                    //Debug_Print();
                } else {
                    printf("=== DEBUG COMPLETE: %d groups captured ===\r\n", DEBUG_CAPTURE_GROUPS);
                    g_pauseMainLoop = true;
                }
#endif
				Debug_Print();
                OutputTask();
                LedTask();
                //printf("V_OUT: %d\r\n", V_OUT);
            }
        }
    }

    /* pause program here after exiting main loop */
    while (1) {
		
    }
}



