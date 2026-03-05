#ifndef __TASK_OUTPUT_H
#define __TASK_OUTPUT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define V_SET               400U        // Judge presence threshold (mV), can be adjusted based on testing results
#define V_HYS               50U         // Hysteresis for presence detection (mV)
#define OOR_ENTER_COUNT     5U          // Number of consecutive invalid samples to enter OUT_OF_RANGE state

typedef enum {
    NO_OBJECT = 0,
    OBJECT_PRESENT  = 1,
    OUT_OF_RANGE = 2,
    SHORT_CIRCUIT  = 3
} output_state_t;

extern __IO output_state_t g_outputState;

void OutputTask(void);

#ifdef __cplusplus
}
#endif

#endif
