#ifndef __FILTER_H
#define __FILTER_H

#ifdef cplusplus
extern "C" {
#endif

#include <stdint.h>

#define KFN					6

	typedef enum {
		kF0 = 0,
		kF1,
		kF2,
		kF3,
		kF4,
		kF5
	} kf_t;

	extern volatile int g_measCnt[];
	extern volatile float g_rawPP[];

	float KalmanFilter(float val, float Q, float R, int index);
	void KalmanFilterS(float* val, float Q, float R, int index);
	void IIR_Init(float val);
	float IIR_Filter(float val);

#ifdef cplusplus
}
#endif

#endif
