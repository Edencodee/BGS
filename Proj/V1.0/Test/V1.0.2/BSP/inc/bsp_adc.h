#ifndef __BSP_ADC_H
#define __BSP_ADC_H


#ifdef __cplusplus
extern "C" {
#endif

#include "gd32c2x1.h"
#include "main.h"

/* ADC buffer configuration --------------------------------- */
#define ADC_CHANNEL                 ADC_CHANNEL_1
#define ADC_DMA_CHANNEL             DMA_CH0

/* Sample point range: 1~100 */
#define SAMPLE_POINT		        10U

#define ADC_SAMPLING_FREQ 			500000  //500KHz

extern __IO int16_t g_A_Buffer[SAMPLE_POINT];
extern __IO int16_t g_B_Buffer[SAMPLE_POINT];

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
void ADC_StartSampling(void);
void ADC_StopSampling(void);
void ADC_SwitchBuffer(int16_t *g_adcCaptureBuffer);
/* ---------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif
