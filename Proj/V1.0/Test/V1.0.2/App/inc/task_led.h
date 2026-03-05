#ifndef __TASK_LED_H
#define __TASK_LED_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


typedef enum {
    DARK_ON = 0,
    LIGHT_ON = 1
} led_mode_t;

typedef enum {
	kLedOff = 0,
	kLedOn,
	kLedToggle,
	kLedNone
} led_state_t;

void LedTask_Init(void);
void LED_SetPwr(led_state_t state);
void LED_SetWork(led_state_t state);
void LedTask(void);


#ifdef __cplusplus
}
#endif

#endif
