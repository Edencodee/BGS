#ifndef __USER_TIMER_H
#define __USER_TIMER_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32c2x1.h"

#define EXTLS_PIN						GPIO_PIN_8
#define EXTLS_PORT					GPIOA
#define EXTLS_CLK						RCU_GPIOA
#define EXTLS_AF						GPIO_AF_15

#define EXTHS_PIN           GPIO_PIN_9
#define EXTHS_PORT          GPIOA
#define EXTHS_CLK						RCU_GPIOA
#define EXTHS_AF						GPIO_AF_0

void EXTLS();
void EXTHS();

#ifdef cplusplus
}
#endif

#endif /* USER_TIMER_H */
