#include "main.h"

#define BPS_N 7
#define AUTO_DSP_REV 1

void SplineInterpCali (void)
{
	ele_t yf[PN];
	for (int i = 0; i < PN; i++) {
		if (isnan(g_spara.xs[i])) {
			return;
		}
		if (isnan(g_spara.ys[i])) {
			return;
		}
		yf[i] = (ele_t)g_spara.ys[i];
	}

	SplineInterp((ele_t*)g_spara.xs, \
	             (ele_t*)yf, \
	             (ele_t*)g_scoef.a,  \
	             (ele_t*)g_scoef.b,  \
	             (ele_t*)g_scoef.c,  \
	             (ele_t*)g_scoef.d);
}

void FlashTask (void)
{
	const uint32_t baudrates[BPS_N] = {
		9600, 19200, 38400, 57600, 115200, 256000, 500000
	};

	/* 1. Reg write */
	if (ChkFlag(kFlagWriteReg) && OledChkBsy()) {
		/* 1.1. check reset ------------------------------------------------------------- */
		if (g_reg[kRegReset] == kReset) {
			FLASH_RegReinit();
			__set_FAULTMASK(1);
			NVIC_SystemReset();
		}
		/* 1.2. sync bps ---------------------------------------------------------------- */
		if (g_sreg.bps != baudrates[g_sreg.bpsEnum]) {
			if (g_isSetByKey) {
				g_isSetByKey = false;
				g_sreg.bps = baudrates[g_sreg.bpsEnum];
			} else {
				for (int i = 0; i < BPS_N; i++) {
					if (baudrates[i] == g_sreg.bps) {
						g_sreg.bpsEnum = i;
						break;
					}
				}
			}
			usart_baudrate_set(USER_COM1, g_sreg.bps);
		}
		/* ------------------------------------------------------------------------------ */
		/* 1.3. zero check -------------------------------------------------------------- */
		if (g_sreg.zero > 0) {
			g_sreg.zero = 0;
#if AUTO_DSP_REV
			g_sreg.dsp = kDpReverse;
#endif
			if (g_sreg.dsp == kDpReverse) {
				if (g_sreg.offs != 0) {
					g_sreg.offs = 0; /* clear zero */
					wlog("clear zero");
				} else {
					g_sreg.offs = g_sreg.dist10; /* set zero */
					wlog("set zero");
				}
			} else {
				ClrFlag(kFlagWriteReg); /* set zero only in reverse-disp mode */
			}
		}
		/* ------------------------------------------------------------------------------ */
		/* 1.4. analog check ------------------------------------------------------------ */
		if (g_sreg.analog != g_rom[kRegAnalog]) {
			GPIO_AnalogSel(g_sreg.analog);
		}
		/* ------------------------------------------------------------------------------ */
		/* 1.5. out check --------------------------------------------------------------- */
		if (g_sreg.out != g_rom[kRegOut]) {
			ResetWaveType();
		}
		/* ------------------------------------------------------------------------------ */
		/* 1.6. dspDir check ------------------------------------------------------------ */
		if (g_sreg.dspDir != g_rom[kRegDspDir]) {
			OLED_SetDir((dsp_dir_t)g_sreg.dspDir);
		}
		/* ------------------------------------------------------------------------------ */

		/* 1.7. write execute */
		FLASH_RegWrite();
		wlog("FLASH_RegWrite");
		ClrFlag(kFlagWriteReg);
	}

	/* 2. Coef write */
	if (ChkFlag(kFlagWriteCoef)) {
		FLASH_CoefWrite();
		wlog("FLASH_CoefWrite");
		ClrFlag(kFlagWriteCoef);
	}

	/* 3. Para write */
	if (ChkFlag(kFlagWritePara)) {
		/* 3.1. get xs[n] */
		int n = g_spara.currIdx;
		if (n != -1) { 		/* n = -1 means not xs, ys */
			if (n < PN) {
				g_spara.xs[n] = g_filtPos;
			} else {		/* n >= PN means calculate */
				(*(u32_t*)&g_spara.flag).buf16[1] = kNPIntpCali;
				SplineInterpCali(); // get a b c d
				SetFlag(kFlagWriteCoef); // save a b c d
			}
			g_spara.currIdx = -1;
		}

		/* 3.2. write execute */
		FLASH_ParaWrite();
		wlog("FLASH_ParaWrite");
		ClrFlag(kFlagWritePara);
	}
}
