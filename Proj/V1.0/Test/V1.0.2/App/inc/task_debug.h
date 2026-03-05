#ifndef TASK_DEBUG_H
#define TASK_DEBUG_H

#include <stdint.h>
#include <stdbool.h>

/* ==================== Debug Configuration ==================== */
#define DEBUG_MODE_ENABLE      		0U
#define DEBUG_CAPTURE_GROUPS   		10U
#define DEBUG_USE_DEFAULT_CONFIG  	0U   /* 1=Force using default config 0=Load from Flash  */

/* ==================== Timing Measurement ==================== */
#define ENABLE_TIMING_MEASUREMENT  0U    /* 1=Enable timing measurement, 0=Disable */

/* Timing measurement structure */
typedef struct {
    uint32_t t0_start;          /* 时间戳：开始采样 */
    uint32_t t1_sample_done;    /* 时间戳：采样完成 */
    uint32_t t2_analyze_done;   /* 时间戳：分析完成 */
    uint32_t t3_output_done;    /* 时间戳：输出完成 */
    
    uint32_t sampling_time;     /* 采样耗时 (us) = t1 - t0 */
    uint32_t analyze_time;      /* 分析耗时 (us) = t2 - t1 */
    uint32_t output_time;       /* 输出耗时 (us) = t3 - t2 */
    uint32_t total_time;        /* 总耗时 (us) = t3 - t0 */
} timing_stats_t;

/* Export timing stats */
extern timing_stats_t g_timing_stats;

/* Function prototypes */
void Debug_Print(void);
void Debug_PrintTimingStats(void);
uint32_t Debug_GetTimestampUs(void);

/* Inline helper for recording timestamps */
static inline void Debug_RecordTimestamp_Start(void) {
#if ENABLE_TIMING_MEASUREMENT
    extern timing_stats_t g_timing_stats;
    g_timing_stats.t0_start = Debug_GetTimestampUs();
#endif
}

static inline void Debug_RecordTimestamp_SampleDone(void) {
#if ENABLE_TIMING_MEASUREMENT
    extern timing_stats_t g_timing_stats;
    g_timing_stats.t1_sample_done = Debug_GetTimestampUs();
#endif
}

static inline void Debug_RecordTimestamp_AnalyzeDone(void) {
#if ENABLE_TIMING_MEASUREMENT
    extern timing_stats_t g_timing_stats;
    g_timing_stats.t2_analyze_done = Debug_GetTimestampUs();
#endif
}

static inline void Debug_RecordTimestamp_OutputDone(void) {
#if ENABLE_TIMING_MEASUREMENT
    extern timing_stats_t g_timing_stats;
    g_timing_stats.t3_output_done = Debug_GetTimestampUs();
#endif
}

static inline void Debug_CalculateAndPrintTiming(void) {
#if ENABLE_TIMING_MEASUREMENT
    extern timing_stats_t g_timing_stats;
    g_timing_stats.sampling_time = g_timing_stats.t1_sample_done - g_timing_stats.t0_start;
    g_timing_stats.analyze_time = g_timing_stats.t2_analyze_done - g_timing_stats.t1_sample_done;
    g_timing_stats.output_time = g_timing_stats.t3_output_done - g_timing_stats.t2_analyze_done;
    g_timing_stats.total_time = g_timing_stats.t3_output_done - g_timing_stats.t0_start;
    Debug_PrintTimingStats();
#endif
}

#endif /* TASK_DEBUG_H */
