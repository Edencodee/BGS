#ifndef __TASK_OUTPUT_H
#define __TASK_OUTPUT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


typedef enum {
    NO_OBJECT = 0,
    OBJECT_PRESENT  = 1,
    SHORT_CIRCUIT  = -1
} output_state_t;

extern __IO output_state_t g_outputState;

void OutputTask(void);

#ifdef __cplusplus
}
#endif

#endif
