#ifndef __BSP_ADC_H
#define __BSP_ADC_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "gd32c2x1.h"

/* ADC buffer configuration --------------------------------- */
#define ADC_BUF_LEN         10U
extern uint16_t adc_value_buf[ADC_BUF_LEN];
extern uint8_t adc_buf_index;

/* function declaration ------------------------------- */
void ADC_UserInit(void);
uint16_t ADC_GetValue(void);

/* ---------------------------------------------------- */



#ifdef __cplusplus
}
#endif

#endif
