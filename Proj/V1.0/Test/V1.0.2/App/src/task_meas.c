#include "main.h"


void MeasureTask(void)
{
    /* Prevent EXTI from triggering again during SwitchBuffer execution */
    g_isSampleDone = false;

    /* g_plsState has been set in EXTI ISR */
    if (g_plsState == kHigh) {
        /* Clear old B Done flag before sampling B */
        g_isB_Done = false;
        ADC_SwitchBuffer((int16_t *)g_B_Buffer);
    } else {
        /* Clear old A Done flag before sampling A */
        g_isA_Done = false;
        ADC_SwitchBuffer((int16_t *)g_A_Buffer);
    }
    /* Start ADC sampling (timer and DMA) */
    ADC_StartSampling();
}


