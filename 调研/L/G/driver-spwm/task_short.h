#ifndef __TASK_SHORT_H
#define __TASK_SHORT_H

#ifdef cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "gd32f4xx.h"

	extern __IO bool g_isShort;

	void ShortChkTask (void);

#ifdef cplusplus
}
#endif

#endif
