#ifndef __SYS_TICK_H
#define __SYS_TICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "n32l40x.h"

/* configure systick */
void systick_config(void);
/* delay a time in milliseconds */
void delay_1ms(uint32_t count);
/* delay decrement */
void delay_decrement(void);
/* get systick count */
uint32_t getTick(void);

#ifdef __cplusplus
}
#endif

#endif /* SYS_TICK_H */
