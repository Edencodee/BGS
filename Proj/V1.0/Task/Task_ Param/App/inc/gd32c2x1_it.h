#ifndef GD32C2X1_IT_H
#define GD32C2X1_IT_H

#include "gd32c2x1.h"

/* function declarations */
/* this function handles NMI exception */
void NMI_Handler(void);
/* this function handles HardFault exception */
void HardFault_Handler(void);
/* this function handles SVC exception */
void SVC_Handler(void);
/* this function handles PendSV exception */
void PendSV_Handler(void);
/* this function handles SysTick exception */
void SysTick_Handler(void);
/* this function handles EXTI line5 to line9 interrupt */
void EXTI5_9_IRQHandler(void);

#endif /* GD32C2X1_IT_H */
