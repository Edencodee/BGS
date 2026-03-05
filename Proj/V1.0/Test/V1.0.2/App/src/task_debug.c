#include "main.h"

/* ==================== Global Variables ==================== */
timing_stats_t g_timing_stats = {0};

/**
 * @brief  获取当前时间戳（微秒）
 * @retval 当前时间戳（微秒）
 */
uint32_t Debug_GetTimestampUs(void)
{
    return tick_count * 1000U + (1000U - (SysTick->VAL * 1000U) / (SysTick->LOAD + 1));
}

/**
 * @brief  打印调试信息（采样数据和输出电压）
 * @note   显示A/B通道的采样值和最终输出电压
 */
void Debug_Print(void)
{
	printf("SAMPLE_POINT=%u\r\n", (unsigned)SAMPLE_POINT);
    for(uint8_t i = 0; i < SAMPLE_POINT; i++) {
        printf("A[%d]: %d, B[%d]: %d\r\n", i, g_A_Buffer[i], i, g_B_Buffer[i]);
    }
    printf("V_A: %d , V_B: %d , V_OUT: %d \r\n", V_A, V_B, V_OUT);
	printf("\n");
}

/**
 * @brief  打印时间测量统计信息
 * @note   显示采样、分析、输出各阶段耗时以及总耗时
 *         包含Sum Check验证时间一致性
 */
void Debug_PrintTimingStats(void)
{
    uint32_t sum_time = g_timing_stats.sampling_time + g_timing_stats.analyze_time + g_timing_stats.output_time;
    
    printf("========== Timing Statistics ==========\r\n");
    printf("Sampling Time  : %3u.%03u ms (%u us)\r\n", 
           g_timing_stats.sampling_time / 1000U,
           g_timing_stats.sampling_time % 1000U,
           g_timing_stats.sampling_time);
    printf("Analyze Time   : %3u.%03u ms (%u us)\r\n", 
           g_timing_stats.analyze_time / 1000U,
           g_timing_stats.analyze_time % 1000U,
           g_timing_stats.analyze_time);
    printf("Output Time    : %3u.%03u ms (%u us)\r\n", 
           g_timing_stats.output_time / 1000U,
           g_timing_stats.output_time % 1000U,
           g_timing_stats.output_time);
    printf("---------------------------------------\r\n");
    printf("Total Time     : %3u.%03u ms (%u us)\r\n", 
           g_timing_stats.total_time / 1000U,
           g_timing_stats.total_time % 1000U,
           g_timing_stats.total_time);
    printf("Sum Check      : %3u.%03u ms (%u us)\r\n", 
           sum_time / 1000U,
           sum_time % 1000U,
           sum_time);
    printf("=======================================\r\n\n");
}
