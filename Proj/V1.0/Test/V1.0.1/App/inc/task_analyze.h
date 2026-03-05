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

/* valid V_OUT samples for clustering */
#ifndef VOUT_VALID_LEN
#define VOUT_VALID_LEN      20U
#endif

/* window size used to find the most concentrated cluster */
#ifndef VOUT_CLUSTER_WINDOW
#define VOUT_CLUSTER_WINDOW 8U
#endif

/* Ensure discarded samples do not exceed array length / 2 */
#ifdef SAMPLE_POINT
#if (DISCARD_N * 2 >= SAMPLE_POINT)
#error "DISCARD_N too large for SAMPLE_POINT"
#endif
#endif

#if (VOUT_CLUSTER_WINDOW == 0U)
#error "VOUT_CLUSTER_WINDOW must be > 0"
#endif

#if (VOUT_CLUSTER_WINDOW > VOUT_VALID_LEN)
#error "VOUT_CLUSTER_WINDOW must be <= VOUT_VALID_LEN"
#endif


extern int16_t V_A;
extern int16_t V_B;  
extern int16_t V_OUT; 

void AnalyzeTask(void);
/* Fast trimmed mean filtering (integer) */
uint16_t FastTrimmedMean(const volatile uint16_t *buf, uint16_t len, uint16_t discard);

#ifdef __cplusplus
}
#endif

#endif
