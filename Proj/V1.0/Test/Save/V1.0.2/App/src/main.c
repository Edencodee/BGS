#include "main.h"
#include "SEGGER_RTT.h"
#include "debug_config.h"

#define APP_VERSION		"1.0.2"


#define DEBUG_MODE_ENABLE   0U
#define DEBUG_CAPTURE_GROUPS   20U

__IO bool g_isSampleDone = true;
__IO bool g_isStartSampling = false;
__IO bool g_isSysTickInt = false;
__IO bool g_isA_Done = false;   /* A路DMA采样完成标志 */
__IO bool g_isB_Done = false;   /* B路DMA采样完成标志 */


__IO bool g_pauseMainLoop = false; /* 置 true 后退出主循环并暂停程序 */

volatile param_config_t g_param_cfg = PARAM_CONFIG_USER; 

void Debug_Print(void);

int main(void)
{
    uint16_t debug_group_count = 0U;

    printf("App Version: %s\r\n", APP_VERSION);
    printf("Debug Level: %d (0=NONE, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=VERBOSE)\r\n", DEBUG_LEVEL);
    
    BSP_Init();
    Task_Init();
    ISL700_Init(&g_param_cfg.isl700);
    
    /* 初始化统计数据 */
    Debug_ResetStatistics();
    
    delay_1ms(500);
    //start_timing_us();
    
    while (1) {
        if (g_pauseMainLoop) {
            break;
        }

        if (g_isSysTickInt) {
            g_isSysTickInt = false;
            //SysTickTask();
        }

        if(g_isStartSampling) {
            MeasureTask();
            g_isStartSampling = false;
        } 

        /* 仅当A、B两路均采样完成，构成一次完整测量 */
        if(g_isA_Done && g_isB_Done) {
            g_isA_Done = false;
            g_isB_Done = false;
            AnalyzeTask();
            
            /* 仅当平均样本充足时才执行输出任务 */
            if(g_isVoutReady) {
                g_isVoutReady = false;
                
#if DEBUG_MODE_ENABLE
                /* 调试模式：周期性打印并暂停 */
                if (debug_group_count < DEBUG_CAPTURE_GROUPS) {
                    debug_group_count++;
                    printf("\r\n=== Debug Group %d ===\r\n", debug_group_count);
                    Debug_Print();
                    
                    /* 每10组打印一次详细ADC数据（VERBOSE模式） */
                    if (debug_group_count % 10 == 0) {
                        Debug_PrintADCBuffers(g_A_Buffer, g_B_Buffer, SAMPLE_POINT);
                    }
                } else {
                    printf("\r\n=== DEBUG COMPLETE: %d groups captured ===\r\n", DEBUG_CAPTURE_GROUPS);
                    Debug_PrintStatistics();  /* 打印统计汇总 */
                    g_pauseMainLoop = true;
                }
#else
                /* 正常运行模式：周期性打印统计信息 */
                DEBUG_THROTTLE(1000, "\r\n");  /* 每1000次打印一次统计 */
                if (g_debug_stats.total_samples % 1000 == 0 && g_debug_stats.total_samples > 0) {
                    Debug_PrintStatistics();
                }
#endif
                
                /* 正常输出流程 */
                OutputTask();
                LedTask();
                
                //printf("V_OUT: %d\r\n", V_OUT);
                //print_timing_us(stop_timing_us());
            }
            /* 样本积累期：g_isVoutReady=false，跳过输出，继续下一轮采样 */
        }
    }

    /* pause program here after exiting main loop */
    DEBUG_INFO("Main loop exited. Entering idle state...\r\n");
    while (1) {
    }
}

void Debug_Print(void)
{
	printf("SAMPLE_POINT=%u\r\n", (unsigned)SAMPLE_POINT);
    for(uint8_t i = 0; i < SAMPLE_POINT; i++) {
        printf("A[%d]: %d, B[%d]: %d\r\n", i, g_A_Buffer[i], i, g_B_Buffer[i]);
    }
    printf("V_A: %d , V_B: %d , V_OUT: %d \r\n", V_A, V_B, V_OUT);
	printf("\n");
}



