#ifndef __TASK_LED_H
#define __TASK_LED_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "gd32c2x1.h"
#include "stdbool.h"

#define LIGHT_ON            1
#define DARK_ON             0


/* ==================== LED 任务 API ==================== */
void LedTask_Init(void);
void Led_SetPwr(bool on);
void Led_SetWork(bool on);
void Led_SetMeasState(bool active);
void Led_SetMode(bool light_on);
void LedTask(void);


#ifdef __cplusplus
}
#endif

#endif
