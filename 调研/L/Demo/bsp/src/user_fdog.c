#include "main.h"

/* WDG define */
#define FEED_DOG_TMO		1

void FeedDog(void)
{
	static uint32_t s_wdgCnt = 0;
	if (++s_wdgCnt > FEED_DOG_TMO) {
		s_wdgCnt = 0;
		/* Reload IWDG counter */
		IWDG_ReloadKey();
	}
}

void IWDG_Init(void)
{
	/* Enable PWR Clock */
	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
	DBG_ConfigPeriph(DBG_IWDG_STOP, ENABLE);

	/* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
	   dispersion) */
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteConfig(IWDG_WRITE_ENABLE);

	/* IWDG counter clock: LSI/256 */
	IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV256);

	/* Set counter reload value to obtain 1000ms IWDG TimeOut.
	   Counter Reload Value = 1000ms / IWDG counter clock period
	                        = 1000ms * (LSI / 256)
	 */
	IWDG_CntReload(118);	/* 1007ms, between 0 and 0x0FFF */
	/* Reload IWDG counter */
	IWDG_ReloadKey();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
}
