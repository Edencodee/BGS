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
#define DISCARD_N           1U
#endif

/* Ensure discarded samples do not exceed array length / 2 */
#ifdef SAMPLE_POINT
#if (DISCARD_N * 2 >= SAMPLE_POINT)
#error "DISCARD_N too large for SAMPLE_POINT"
#endif
#endif


extern uint16_t V_A;
extern uint16_t V_B;  
extern uint16_t V_OUT; 

void AnalyzeTask(void);
/* Fast trimmed mean filtering (integer) */
uint16_t FastTrimmedMean(const volatile uint16_t *buf, uint16_t len, uint16_t discard);

#ifdef __cplusplus
}
#endif

#endif
