#ifndef __BSP_EXIT_H
#define __BSP_EXIT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "gd32c2x1.h"


/* PLS_SO Input IO (External Interrupt) --------------- */
#define PLS_SO_PIN                GPIO_PIN_6
#define PLS_SO_PORT               GPIOA
#define PLS_SO_CLK                RCU_GPIOA
#define PLS_SO_EXTI_LINE          EXTI_6
#define PLS_SO_EXTI_PORT_SOURCE   EXTI_SOURCE_GPIOA
#define PLS_SO_EXTI_PIN_SOURCE    EXTI_SOURCE_PIN6
/* ---------------------------------------------------- */

void PLS_SO_InterruptInit(void);


#ifdef cplusplus
}
#endif

#endif
