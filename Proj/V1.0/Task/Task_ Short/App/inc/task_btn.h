#ifndef __TASK_BTN_H
#define __TASK_BTN_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "gd32c2x1.h"
#include "stdbool.h"


/* ==================== Button 任务配置 ==================== */
#define BTN_DEBOUNCE_TICKS   20U


/* ==================== Button 任务 API ==================== */
void BtnTask_Init(void);
void BtnTask(void);
bool Btn_GetLedMode(void);


#ifdef __cplusplus
}
#endif

#endif
