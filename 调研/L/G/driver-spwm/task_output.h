#ifndef __TASK_OUTPUT_H
#define __TASK_OUTPUT_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32f4xx.h"

	extern __IO int32_t g_outState;

	void OutputTask (void);

#ifdef cplusplus
}
#endif

#endif
