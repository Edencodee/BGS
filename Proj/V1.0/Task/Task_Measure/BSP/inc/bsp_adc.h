#ifndef __BSP_ADC_H
#define __BSP_ADC_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "gd32c2x1.h"
#include "gd32c2x1_dma.h"

/* ADC buffer configuration --------------------------------- */
#define ADC_CHANNEL         ADC_CHANNEL_1
#define ADC_DMA_CHANNEL     DMA_CH0

#define SAMPLE_POINT		10U
extern __IO uint16_t g_A_Buffer[SAMPLE_POINT];
extern __IO uint16_t g_B_Buffer[SAMPLE_POINT];

/* -------------------------------------------------------- */

/* ADC IO --------------------------------------------- */
#define ADC_PIN           			GPIO_PIN_1	
#define ADC_PORT     				GPIOA
#define ADC_PIN_CLK      			RCU_GPIOA
/* ---------------------------------------------------- */


/* function declaration ------------------------------- */
void RCU_Config(void);
void GPIO_Config(void);
void DMA_Config(void);
void ADC_TIMER_Config(void);
void ADC_Config(void);
void ADC_UserInit(void);
void ADC_Sampling_Start(void);
void ADC_Sampling_Stop(void);
void SwitchAdcBuffer(uint16_t *g_adcCaptureBuffer);
/* ---------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif
