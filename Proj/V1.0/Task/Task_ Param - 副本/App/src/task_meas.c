#include "main.h"


void MeasureTask(void)
{
    g_plsState = (SET == gpio_input_bit_get(PLS_SO_PORT, PLS_SO_PIN)) ? kHigh : kLow;
	
	if (g_plsState == kHigh) {
		ADC_SwitchBuffer((uint16_t *)g_B_Buffer);
	} else {
		ADC_SwitchBuffer((uint16_t *)g_A_Buffer);
	}

	ADC_StartSampling();
}

