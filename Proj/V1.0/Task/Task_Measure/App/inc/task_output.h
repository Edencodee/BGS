#ifndef __TASK_OUTPUT_H
#define __TASK_OUTPUT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "gd32c2x1.h"
#include "stdbool.h"

extern __IO bool g_outState;

/* ==================== Output 任务 API ==================== */
void OutputTask_Init(void);
void OutputTask_SetPresence(bool presence);
bool OutputTask_GetPresence(void);
void OutputTask(void);




#ifdef __cplusplus
}
#endif

#endif
