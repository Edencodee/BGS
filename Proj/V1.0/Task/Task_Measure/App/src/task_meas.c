#include "task_meas.h"


void MeasTask(void)
{
    printf("MeasTask: Start ADC Sampling\r\n");
    g_plsState = (SET == gpio_input_bit_get(PLS_SO_PORT, PLS_SO_PIN)) ? kHigh : kLow;
	if (g_plsState == kLow) {
		SwitchAdcBuffer((uint16_t *)g_B_Buffer);
	} else {
		SwitchAdcBuffer((uint16_t *)g_A_Buffer);
	}
    
	ADC_Sampling_Start();
}

