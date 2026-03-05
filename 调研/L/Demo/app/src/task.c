#include "main.h"

#define FLASH_WRITE_CHK		200
#define cnt10ms_CHK			10

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
void Timer2UpdateCallBack (void)
{
//	ShortChkTask();
}
/* ------------------------------------------------------------------------------------------ */

/* sysTick interrupt call back -------------------------------------------------------------- */
void SysTickCallBack (void)
{
	static int32_t cnt10ms = 0;
	static int32_t s_WriteFlashCnt = 0;

	if (!ChkFlag(kFlagInit)) {
		return;
	}
	/* 1. feed dog -------------------------------------------------------------------------- */
	if (LoopCntInc(&cnt10ms, cnt10ms_CHK)) {
		IWDG_ReloadKey();
	}
	/* 2. btn & led routine ----------------------------------------------------------------- */
	BtnTask();
	LedTask();
	/* -------------------------------------------------------------------------------------- */
	/* 3. flash write ----------------------------------------------------------------------- */
	if (LoopCntInc(&s_WriteFlashCnt, FLASH_WRITE_CHK)) {
		FlashTask();
	}
}
/* ------------------------------------------------------------------------------------------ */

/* recv idle call back----------------------------------------------------------------------- */
void RecvIdleCallBack (void* userData, uint16_t len)
{
//	uint8_t *p = userData;

//	if(AtspChk(p)) {
//		Atsp(p);
//	} else {
//		MODBUS_RecvHandler(p, len);
//	}
}
/* ------------------------------------------------------------------------------------------ */
