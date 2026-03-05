#ifndef __DEBUG_CONFIG_H
#define __DEBUG_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* ============ 调试级别定义 ============ */
#define DEBUG_LEVEL_NONE     0U  /* 关闭所有调试输出 */
#define DEBUG_LEVEL_ERROR    1U  /* 仅错误 */
#define DEBUG_LEVEL_WARN     2U  /* 错误+警告 */
#define DEBUG_LEVEL_INFO     3U  /* 错误+警告+信息 */
#define DEBUG_LEVEL_DEBUG    4U  /* 错误+警告+信息+调试 */
#define DEBUG_LEVEL_VERBOSE  5U  /* 所有信息（含详细ADC数据） */

/* ============ 全局调试级别配置 ============ */
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL  DEBUG_LEVEL_INFO  /* 默认：INFO级别 */
#endif

/* ============ 分模块调试开关 ============ */
#define DEBUG_MODULE_ADC        1U  /* ADC采样调试 */
#define DEBUG_MODULE_ANALYZE    1U  /* 数据分析调试 */
#define DEBUG_MODULE_OUTPUT     1U  /* 输出控制调试 */
#define DEBUG_MODULE_STAT       1U  /* 统计信息 */

/* ============ 限频打印配置 ============ */
#define DEBUG_PRINT_INTERVAL_DEFAULT   100U  /* 默认每100次打印一次 */

/* ============ 调试宏定义 ============ */
#if (DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)
    #define DEBUG_ERROR(fmt, ...) printf("[ERROR] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_ERROR(fmt, ...)
#endif

#if (DEBUG_LEVEL >= DEBUG_LEVEL_WARN)
    #define DEBUG_WARN(fmt, ...) printf("[WARN] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_WARN(fmt, ...)
#endif

#if (DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
    #define DEBUG_INFO(fmt, ...) printf("[INFO] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_INFO(fmt, ...)
#endif

#if (DEBUG_LEVEL >= DEBUG_LEVEL_DEBUG)
    #define DEBUG_LOG(fmt, ...) printf("[DEBUG] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_LOG(fmt, ...)
#endif

#if (DEBUG_LEVEL >= DEBUG_LEVEL_VERBOSE)
    #define DEBUG_VERBOSE(fmt, ...) printf("[VERBOSE] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_VERBOSE(fmt, ...)
#endif

/* ============ 限频打印宏 ============ */
/**
 * @brief  限频打印宏（每N次调用打印一次）
 * @param  interval: 打印间隔（每N次打印一次）
 * @param  fmt: 格式化字符串
 * @usage  DEBUG_THROTTLE(100, "Sample: V_OUT=%d\r\n", V_OUT);
 */
#define DEBUG_THROTTLE(interval, fmt, ...) \
    do { \
        static uint32_t _counter = 0; \
        if (++_counter >= (interval)) { \
            printf(fmt, ##__VA_ARGS__); \
            _counter = 0; \
        } \
    } while(0)

/* ============ 模块调试宏 ============ */
#if DEBUG_MODULE_ADC
    #define DEBUG_ADC(fmt, ...) DEBUG_LOG("[ADC] " fmt, ##__VA_ARGS__)
    #define DEBUG_ADC_VERBOSE(fmt, ...) DEBUG_VERBOSE("[ADC] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_ADC(fmt, ...)
    #define DEBUG_ADC_VERBOSE(fmt, ...)
#endif

#if DEBUG_MODULE_ANALYZE
    #define DEBUG_ANALYZE(fmt, ...) DEBUG_LOG("[ANALYZE] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_ANALYZE(fmt, ...)
#endif

#if DEBUG_MODULE_OUTPUT
    #define DEBUG_OUTPUT(fmt, ...) DEBUG_LOG("[OUTPUT] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_OUTPUT(fmt, ...)
#endif

#if DEBUG_MODULE_STAT
    #define DEBUG_STAT(fmt, ...) DEBUG_INFO("[STAT] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_STAT(fmt, ...)
#endif

/* ============ 统计数据结构 ============ */
typedef struct {
    uint32_t total_samples;        /* 总采样次数 */
    uint32_t valid_samples;        /* 有效样本数 */
    uint32_t invalid_samples;      /* 无效样本数 */
    uint32_t out_of_range_count;   /* 超量程次数 */
    uint32_t object_present_count; /* 检测到物体次数 */
    uint32_t state_transitions;    /* 状态切换次数 */
} debug_statistics_t;

extern debug_statistics_t g_debug_stats;

/* ============ 统计功能 API ============ */
void Debug_ResetStatistics(void);
void Debug_PrintStatistics(void);
void Debug_UpdateStatistics(bool is_valid, bool is_ready);

/* ============ 实用调试函数 ============ */
void Debug_PrintADCBuffers(const volatile uint16_t *buf_a, const volatile uint16_t *buf_b, uint8_t len);
void Debug_PrintVoltages(uint16_t v_a, uint16_t v_b, uint16_t v_out, bool is_valid);
void Debug_PrintOutputState(void);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_CONFIG_H */
