#include "main.h"


void MeasureTask(void)
{
    g_plsState = (SET == gpio_input_bit_get(PLS_SO_PORT, PLS_SO_PIN)) ? kHigh : kLow;
	
	if (g_plsState == kHigh) {
		ADC_SwitchBuffer((int16_t *)g_B_Buffer);
	} else {
		ADC_SwitchBuffer((int16_t *)g_A_Buffer);
	}

	ADC_StartSampling();
}



/* @brief  delay a time in microseconds
 * @param count: delay time in microseconds
 * @note   基于SysTick计数器实现的精确微秒延迟
 *         适用于需要高精度短延迟的场合（如GPIO时序控制）
 */

