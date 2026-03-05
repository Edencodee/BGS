#ifndef __FILTER_H
#define __FILTER_H

#ifdef cplusplus
extern "C" {
#endif

#include "main.h"

	#define BG_NOISE			5 /* 50 or others, According to accuracy */
	#define NOISE_RATIO			50 >> 10

	#define LONG_DST			1300

	#define FILT_Q				5
	#define FILT_R				1000

	void GetPP (uint16_t sig);
	void KalmanFilterS(int* val, int Q, int R);

#ifdef cplusplus
}
#endif

#endif
