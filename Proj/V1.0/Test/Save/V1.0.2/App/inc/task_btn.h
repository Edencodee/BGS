#ifndef __TASK_BTN_H
#define __TASK_BTN_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define BTN_DEBOUNCE_TICKS   20U

/* BTN Input typedef ----------------------------------- */
typedef enum {
    BTN_RELEASED = 0,
    BTN_PRESSED = 1
} btn_state_t;

void BtnTask(void);
void BtnTask_Init(void);
btn_state_t Btn_GetState (void);

#ifdef __cplusplus
}
#endif

#endif
