#ifndef __TASK_ANALYZE_H
#define __TASK_ANALYZE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
	
#define VOLTAGE             3300UL        // reference voltage in mV
#define ADC_RESOLUTION      4096UL        // 12-bit ADC resolution
/* V_SET moved to param config (Flash stored) */

/*discard n samples after sorting, then average */
#ifndef DISCARD_N
#define DISCARD_N           2U
#endif

/* Ensure discarded samples do not exceed array length / 2 */
#ifdef SAMPLE_POINT
#if (DISCARD_N * 2 >= SAMPLE_POINT)
#error "DISCARD_N too large for SAMPLE_POINT"
#endif
#endif

/* Number of samples to average for final V_OUT output (1=no average, N>1=average) */
#define VOUT_AVERAGE_COUNT  2U


extern uint16_t V_A;
extern uint16_t V_B;
extern uint16_t V_OUT;
extern bool g_isVoutReady;
extern bool g_isVoutValid;

void AnalyzeTask(void);
uint16_t FastTrimmedMean(const volatile uint16_t *buf, uint16_t len, uint16_t discard);
bool ProcessVoutAverage(uint16_t vout_sample, uint16_t *final_vout);

#ifdef __cplusplus
}
#endif

#endif
