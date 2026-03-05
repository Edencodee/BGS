#include "debug_config.h"
#include "main.h"

/* 全局统计数据 */
debug_statistics_t g_debug_stats = {0};

/**
 * @brief  重置统计数据
 */
void Debug_ResetStatistics(void)
{
    g_debug_stats.total_samples = 0;
    g_debug_stats.valid_samples = 0;
    g_debug_stats.invalid_samples = 0;
    g_debug_stats.out_of_range_count = 0;
    g_debug_stats.object_present_count = 0;
    g_debug_stats.state_transitions = 0;
}

/**
 * @brief  打印统计汇总信息
 */
void Debug_PrintStatistics(void)
{
    DEBUG_STAT("======== Statistics Summary ========\r\n");
    DEBUG_STAT("Total Samples:   %lu\r\n", g_debug_stats.total_samples);
    DEBUG_STAT("Valid Samples:   %lu (%.1f%%)\r\n", 
               g_debug_stats.valid_samples,
               g_debug_stats.total_samples > 0 ? 
               (float)g_debug_stats.valid_samples * 100.0f / g_debug_stats.total_samples : 0.0f);
    DEBUG_STAT("Invalid Samples: %lu (%.1f%%)\r\n", 
               g_debug_stats.invalid_samples,
               g_debug_stats.total_samples > 0 ? 
               (float)g_debug_stats.invalid_samples * 100.0f / g_debug_stats.total_samples : 0.0f);
    DEBUG_STAT("Out of Range:    %lu\r\n", g_debug_stats.out_of_range_count);
    DEBUG_STAT("Object Detected: %lu\r\n", g_debug_stats.object_present_count);
    DEBUG_STAT("State Changes:   %lu\r\n", g_debug_stats.state_transitions);
    DEBUG_STAT("====================================\r\n");
}

/**
 * @brief  更新统计数据（在 AnalyzeTask 中调用）
 * @param  is_valid: 当前样本是否有效
 * @param  is_ready: 数据是否就绪（平均完成）
 */
void Debug_UpdateStatistics(bool is_valid, bool is_ready)
{
    if (is_ready) {
        g_debug_stats.total_samples++;
    }
    
    if (is_valid) {
        g_debug_stats.valid_samples++;
    } else {
        g_debug_stats.invalid_samples++;
    }
}

/**
 * @brief  打印 ADC 缓冲区数据（VERBOSE模式）
 * @param  buf_a: A相缓冲区
 * @param  buf_b: B相缓冲区
 * @param  len: 缓冲区长度
 */
void Debug_PrintADCBuffers(const volatile uint16_t *buf_a, const volatile uint16_t *buf_b, uint8_t len)
{
    DEBUG_ADC_VERBOSE("ADC Buffers (N=%d):\r\n", len);
    for (uint8_t i = 0; i < len; i++) {
        DEBUG_ADC_VERBOSE("  [%d] A=%4d, B=%4d\r\n", i, buf_a[i], buf_b[i]);
    }
}

/**
 * @brief  打印电压信息（带有效性标识）
 * @param  v_a: A相电压
 * @param  v_b: B相电压
 * @param  v_out: 差分输出电压
 * @param  is_valid: 是否有效
 */
void Debug_PrintVoltages(uint16_t v_a, uint16_t v_b, uint16_t v_out, bool is_valid)
{
    if (is_valid) {
        DEBUG_ANALYZE("V_A=%4d mV, V_B=%4d mV, V_OUT=%4d mV [VALID]\r\n", 
                     v_a, v_b, v_out);
    } else {
        DEBUG_WARN("V_A=%4d mV, V_B=%4d mV [INVALID: V_B < V_A]\r\n", 
                   v_a, v_b);
    }
}

/**
 * @brief  打印输出状态信息
 */
void Debug_PrintOutputState(void)
{
    extern __IO output_state_t g_outputState;
    
    const char *state_str = "UNKNOWN";
    switch (g_outputState) {
        case NO_OBJECT:      state_str = "NO_OBJECT";      break;
        case OBJECT_PRESENT: state_str = "OBJECT_PRESENT"; break;
        case OUT_OF_RANGE:   state_str = "OUT_OF_RANGE";   break;
        case SHORT_CIRCUIT:  state_str = "SHORT_CIRCUIT";  break;
    }
    
    DEBUG_OUTPUT("State: %s\r\n", state_str);
}
