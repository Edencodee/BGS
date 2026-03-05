/**
 * @file n32l40x_it.h
 * @author Nations
 * @version V1.2.2
 *
 */
#ifndef __N32L40X_IT_H__
#define __N32L40X_IT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32l40x.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void TIM2_IRQHandler(void);
void USART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __N32L40X_IT_H__ */
