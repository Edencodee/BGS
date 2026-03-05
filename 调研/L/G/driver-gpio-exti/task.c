#include "main.h"

#define FLASH_WRITE_CHK		200
#define FWDGT_CHK			1

__IO bool g_isLogEn = true;

/* software init ---------------------------------------------------------------------------- */
void AppInit (void)
{
	/* 1. menu init ------------------------------------------------------------------------- */
	MenuInit();
	/* 2. range and threshold --------------------------------------------------------------- */
	u32_t flag = {.word = g_spara.flag};
	g_isLogEn = (flag.buf[1] == 1) ? true : false;
	uint8_t type = flag.buf[0] - 1;
	const int32_t range_2[3] = {5000, 10000, 20000};
	sregMax.q1thr1 =  range_2[type];
	sregMax.q1thr2 =  range_2[type];
	sregMax.q2thr1 =  range_2[type];
	sregMax.q2thr2 =  range_2[type];
	sregMin.q1thr1 = -range_2[type];
	sregMin.q1thr2 = -range_2[type];
	sregMin.q2thr1 = -range_2[type];
	sregMin.q2thr2 = -range_2[type];
	const uint32_t range[3] = {10000, 20000, 40000};
	g_sreg.range = range[type];
	const uint32_t measD[3] = {30000, 50000, 85000};
	g_sreg.measDist = measD[type];
	/* 3. wave type setting ----------------------------------------------------------------- */
	ResetWaveType();
	/* 4. cpuTs ----------------------------------------------------------------------------- */
	CpuTsTmrInit();
	uint32_t t1 = CpuTsTmrRead();
	delay_1ms(10);
	uint32_t t2 = CpuTsTmrRead();
	wlog("CpuTs:");
	info("SystemCoreClock = %d", SystemCoreClock);
	info("Delay_ms(100) time: %fus", (t2 - t1) * TS);
	/* 5. cali para ------------------------------------------------------------------------- */
	wlog("CALI PARA:");
	wlog("flag: %x", g_spara.flag);
	wlog("offs: %d", g_spara.offs10);
	wlog("xs ys:");
	for (int i = 0; i < PN; i++) {
		if (!isnan(g_spara.xs[i])) {
			info("%02d %.2f\t%d", i, g_spara.xs[i], g_spara.ys[i]);
		}
	}
}
/* ------------------------------------------------------------------------------------------ */

/* host debug ------------------------------------------------------------------------------- */
static void Debug (uint16_t* p)
{
	const uint8_t head[] = {0xA5, 0x5A};

	UART_SEND_DATA((uint8_t*)head, 2);
	for (int i = 0; i < PIXELS; i++) {
		u16_t temp = {.word = p[i] >> 1};
		u16_t ps = {0};
		ps.buf[kLsb] = temp.buf[kMsb];
		ps.buf[kMsb] = temp.buf[kLsb];
		UART_SEND_DATA(ps.buf, 2);
	}

	int maxIdx;
	PeakFind_AdjustIntT(p, PIXELS, &maxIdx);
}
/* ------------------------------------------------------------------------------------------ */

/* csi-frame handers, called every sample period -------------------------------------------- */
void HandleCSIFrame (uint16_t* p)
{
	if (ChkFlag(kFlagDebugW)) {
		Debug(p);
		ClrFlag(kFlagDebugW);
	} else {
		static uint32_t t1Last = 0;
		static int s_1sCnt = 0;

		uint32_t t1 = CpuTsTmrRead();
		PosMeasure(p);
		uint32_t t2 = CpuTsTmrRead();
		Pos2Dist();
		DacHandler();
		OutputTask();
		if (g_sreg.contiune) { /* modbus contiune mode */
			MODBUS_ContiuneOut();
		}
		uint32_t t3 = CpuTsTmrRead();

		/* measure time debug */
		if (CntInc(&s_1sCnt, 2000)) {
			if (s_1sCnt == 2001) {
				s_1sCnt++; // print only once, s_1sCnt == 2002 finally
				wlog("runtime:");
				info("PosMeasure time: %fus", (t2 - t1) * TS);
				info("routine time: %fus", (t3 - t2) * TS);
				info("frame time: %fus", (t1 - t1Last) * TS);
			}
		}
		t1Last = t1;
	}
}

/* timer0 update call back ------------------------------------------------------------------ */
void Timer3UpdateCallBack (void)
{
	ShortChkTask();
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
		fwdgt_counter_reload();
	}
	/* -------------------------------------------------------------------------------------- */
	/* 2. key & led routine ----------------------------------------------------------------- */
	BtnTask();
	LedTask();
	InputTask();
	/* -------------------------------------------------------------------------------------- */
	/* 3. oled routine ---------------------------------------------------------------------- */
	OledTask((uint16_t*)g_inaAdcBuf);
	/* -------------------------------------------------------------------------------------- */
	/* 4. flash write ----------------------------------------------------------------------- */
	if (LoopCntInc(&s_200msCnt, FLASH_WRITE_CHK)) {
		FlashTask();
	}
}
/* ------------------------------------------------------------------------------------------ */

/* recv idle call back----------------------------------------------------------------------- */
void RecvTimeoutCallBack (void* userData, uint16_t len)
{
	uint8_t *p = userData;

	if(AtspChk(p)) {
		Atsp(p);
	} else {
		MODBUS_RecvHandler(p, len);
	}
}
/* ------------------------------------------------------------------------------------------ */
