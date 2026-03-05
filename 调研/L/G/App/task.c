#include "main.h"

#define FLASH_WRITE_CHK		200
#define FWDGT_CHK			1

__IO bool g_isLogEn = true;

/* counter increase funtion ----------------------------------------------------------------- */
bool LoopCntInc (int32_t *cnt, int32_t max)
{
	if (++(*cnt) > max) {
		*cnt = 0;
		return true;
	}
	return false;
}

bool CntInc (int32_t *cnt, int32_t max)
{
	if (*cnt <= max) {
		(*cnt)++;
		return false;
	}
	return true;
}
/* ------------------------------------------------------------------------------------------ */

/* software init ---------------------------------------------------------------------------- */
void AppInit (void)
{
	/* 1. cpuTs ----------------------------------------------------------------------------- */
	CpuTsTmrInit();
	uint32_t t1 = CpuTsTmrRead();
	delay_1ms(10);
	uint32_t t2 = CpuTsTmrRead();
	wlog("CpuTs:");
	info("SystemCoreClock = %d", SystemCoreClock);
	info("Delay_ms(100) time: %fus", (t2 - t1) * TS);
}
/* ------------------------------------------------------------------------------------------ */

/* timer0 update call back ------------------------------------------------------------------ */
void Timer3UpdateCallBack (void)
{
//	ShortChkTask();
}
/* ------------------------------------------------------------------------------------------ */

/* sysTick interrupt call back -------------------------------------------------------------- */
void SysTickCallBack (void)
{
	static int32_t s_1msCnt = 0;
	static int32_t s_200msCnt = 0;

	if (!ChkFlag(kFlagInit)) {
		return;
	}

	/* 1. feed dog -------------------------------------------------------------------------- */
	if (LoopCntInc(&s_1msCnt, FWDGT_CHK)) {
//		fwdgt_counter_reload();
	}
	/* -------------------------------------------------------------------------------------- */
	/* 2. led routine ----------------------------------------------------------------- */
//	LedTask();
	/* -------------------------------------------------------------------------------------- */
	/* 4. flash write ----------------------------------------------------------------------- */
	if (LoopCntInc(&s_200msCnt, FLASH_WRITE_CHK)) {
//		FlashTask();
	}
}
/* ------------------------------------------------------------------------------------------ */
