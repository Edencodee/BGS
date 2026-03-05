#include "main.h"

#define LONG_LONG_CLICK		7000
#define LONG_CLICK			2000

typedef enum {
	kKsIdle = 0,
	kKsWait1,
	kKsSel,
	kKsWait2
} key_state;

typedef enum {
	kCs1Click1 = 0,
	kCs1Click2,
	kCs1LedTw2s,
	kCs1Err
} cali1_step_t;

typedef enum {
	kCs2Lclick = 0,
	kCs2WaitRelease,
	kCs2Click,
	kCs2LedTw2s,
	kCs2Err
} cali2_step_t;

const int32_t cntThr[8] = {250, 500, 750, 1000, 1250, 1500, 1750, 2000}; /* LED light flashing for two seconds timer */

static inline int32_t DistLmtChk(void)
{
	int32_t res = 0;
	if ((g_sreg.distance < sregMin.cali1p1) || (g_sreg.distance > sregMax.cali1p1)) {
		res = -1;
	}
	return res;
}

static int32_t CaliOkHandler(void)
{
	static __IO int32_t s_okCnt = 0;

	g_sreg.btnLed[kRLed] = kLedOn;
	if (s_okCnt++ < cntThr[0]) {
		g_sreg.btnLed[kBLed] = kLedOn;
		g_sreg.btnLed[kYLed] = kLedOff;
	} else if (s_okCnt < cntThr[1]) {
		g_sreg.btnLed[kBLed] = kLedOff;
		g_sreg.btnLed[kYLed] = kLedOn;
	} else if (s_okCnt < cntThr[2]) {
		g_sreg.btnLed[kBLed] = kLedOn;
		g_sreg.btnLed[kYLed] = kLedOff;
	} else if (s_okCnt < cntThr[3]) {
		g_sreg.btnLed[kBLed] = kLedOff;
		g_sreg.btnLed[kYLed] = kLedOn;
	} else if (s_okCnt < cntThr[4]) {
		g_sreg.btnLed[kBLed] = kLedOn;
		g_sreg.btnLed[kYLed] = kLedOff;
	} else if (s_okCnt < cntThr[5]) {
		g_sreg.btnLed[kBLed] = kLedOff;
		g_sreg.btnLed[kYLed] = kLedOn;
	} else if (s_okCnt < cntThr[6]) {
		g_sreg.btnLed[kBLed] = kLedOn;
		g_sreg.btnLed[kYLed] = kLedOff;
	} else if (s_okCnt < cntThr[7]) {
		g_sreg.btnLed[kBLed] = kLedOff;
		g_sreg.btnLed[kYLed] = kLedOn;
	} else {
		g_sreg.btnLed[kBLed] = kLedOff;
		g_sreg.btnLed[kYLed] = kLedOff;

		s_okCnt = 0;
		return 0;
	}
	return -1;
}

static void CaliErrHandler(void)
{
	static __IO int32_t s_errCnt = 0;

	if (s_errCnt++ < cntThr[0]) {
		g_sreg.btnLed[kYLed] = kLedOn;
		g_sreg.btnLed[kBLed] = kLedOn;
		g_sreg.btnLed[kRLed] = kLedOff;
	} else if (s_errCnt < cntThr[1]) {
		g_sreg.btnLed[kYLed] = kLedOff;
		g_sreg.btnLed[kBLed] = kLedOff;
		g_sreg.btnLed[kRLed] = kLedOn;
	} else if (s_errCnt < cntThr[2]) {
		g_sreg.btnLed[kYLed] = kLedOn;
		g_sreg.btnLed[kBLed] = kLedOn;
		g_sreg.btnLed[kRLed] = kLedOff;
	} else if (s_errCnt < cntThr[3]) {
		g_sreg.btnLed[kYLed] = kLedOff;
		g_sreg.btnLed[kBLed] = kLedOff;
		g_sreg.btnLed[kRLed] = kLedOn;
	} else if (s_errCnt < cntThr[4]) {
		g_sreg.btnLed[kYLed] = kLedOn;
		g_sreg.btnLed[kBLed] = kLedOn;
		g_sreg.btnLed[kRLed] = kLedOff;
	} else if (s_errCnt < cntThr[5]) {
		g_sreg.btnLed[kYLed] = kLedOff;
		g_sreg.btnLed[kBLed] = kLedOff;
		g_sreg.btnLed[kRLed] = kLedOn;
	} else if (s_errCnt < cntThr[6]) {
		g_sreg.btnLed[kYLed] = kLedOn;
		g_sreg.btnLed[kBLed] = kLedOn;
		g_sreg.btnLed[kRLed] = kLedOff;
	} else if (s_errCnt < cntThr[7]) {
		g_sreg.btnLed[kYLed] = kLedOff;
		g_sreg.btnLed[kBLed] = kLedOff;
		g_sreg.btnLed[kRLed] = kLedOn;
	} else {
		s_errCnt = 0;
	}
}

void BtnTask (void)
{
	static __IO int32_t s_longClickCnt = 0;
	static __IO key_state s_settingStep = kKsIdle;
	static __IO int32_t s_longClick = INA;
	static __IO cali1_step_t s_1pCaliStep = kCs1Click1;
	static __IO cali2_step_t s_2pCaliStep = kCs2Lclick;
	static __IO int32_t s_settingTmo = 30 * 100;

	__IO int32_t singleClick = INA;
	__IO int32_t doubleClick = INA;

	/* 1.  button click status check -------------------------------------------------------- */
	GPIO_PbScan(0);
	switch(GPIO_ReadPbStatus(0)) {
		case kLongClick: {
			s_longClick = ACT;
		}
		break;

		case kLongClickRelease: {
			s_longClick = INA;
		}
		break;

		case kSingleClick: {
			singleClick = ACT;
		}
		break;

		case kDoubleClick: {
			doubleClick = ACT;
		}
		break;

		default:
			;
	}
	/* -------------------------------------------------------------------------------------- */
	/* 2.  L-D \ B-F state setting ---------------------------------------------------------- */
	s_longClickCnt = (s_longClick == ACT) ? s_longClickCnt + 1 : 0;
	
	switch(s_settingStep) {
		/* 2.1.  idle, can choose to enter the state setting -------------------------------- */
		case kKsIdle: {
			/* g_sreg.btnLed invalid */
			g_sreg.btnLed[kBLed] = kLedNone;
			g_sreg.btnLed[kYLed] = kLedNone;
			g_sreg.btnLed[kRLed] = kLedNone; /* kLedOff  kLedOn  kLedToggle  kLedNone */

			if (s_longClickCnt > LONG_LONG_CLICK) { /* 8000ms = 1000ms(LONG_PRESSED_TICKS) + 7000ms */
				s_settingStep = kKsWait1;
			}
		}
		break;
		
		/* 2.2.  two LED flashing, release pb to enter state select ------------------------- */
		case kKsWait1: {
			/* 1st sel L-ON BGS */
			g_sreg.btnLed[kYLed] = kLedToggle;
			g_sreg.btnLed[kBLed] = kLedOff;
			g_sreg.btnLed[kRLed] = kLedToggle;

			g_reg[kRegState] = kLonBgs;

			if (s_longClick == INA) {
				s_settingStep = kKsSel;
				s_settingTmo = 30 * 1000;
			}
		}
		break;

		/* 2.3.  state select --------------------------------------------------------------- */
		case kKsSel: {
			if ((s_longClickCnt > LONG_CLICK) || (--s_settingTmo == 0)) { /* 3000ms = 1000ms(LONG_PRESSED_TICKS) + 2000ms */
				s_settingStep = kKsWait2;
				SetFlag(kFlagWriteReg); /* save g_reg[kRegState] to flash */
			} else if (singleClick == ACT) {
				g_reg[kRegState] = ((g_reg[kRegState] + 1) % kStateCnt);
			}
			/* led state -------------------------------------------------------------------- */
			switch(g_reg[kRegState]) {
				case kLonBgs: {
					g_sreg.btnLed[kYLed] = kLedToggle;
					g_sreg.btnLed[kBLed] = kLedToggle;
					g_sreg.btnLed[kRLed] = kLedToggle;
				}
				break;
				case kDonFgs: {
					g_sreg.btnLed[kYLed] = kLedOff;
					g_sreg.btnLed[kBLed] = kLedOff;
					g_sreg.btnLed[kRLed] = kLedToggle;
				}
				break;
				case kLonFgs: {
					g_sreg.btnLed[kYLed] = kLedToggle;
					g_sreg.btnLed[kBLed] = kLedOff;
					g_sreg.btnLed[kRLed] = kLedToggle;
				}
				break;
				case kDonBgs: {
					g_sreg.btnLed[kYLed] = kLedOff;
					g_sreg.btnLed[kBLed] = kLedToggle;
					g_sreg.btnLed[kRLed] = kLedToggle;
				}
				break;
				default:
					;
			}
		}
		break;

		/* 2.4.  already completed state setting, prepare to return idle -------------------- */
		case kKsWait2: {
			for(int i = 0; i < 3; i++) {
				if (g_sreg.btnLed[(led_t)i] == kLedToggle) {
					g_sreg.btnLed[(led_t)i] = kLedOn;
				}
			}
			if (s_longClick == INA) {
				s_settingStep = kKsIdle;
			}
		}
		break;
	}
	/* -------------------------------------------------------------------------------------- */
	if (s_settingStep != kKsIdle) { /* default in kKsSel, TBD */
		if (s_2pCaliStep != kCs2Lclick) {
			s_2pCaliStep = kCs2Lclick;
		}
		if (s_1pCaliStep != kCs1Click1) {
			s_1pCaliStep = kCs1Click1;
		}
		return;
	}
	/* -------------------------------------------------------------------------------------- */
	/* 3.  two point calibrate interface ---------------------------------------------------- */
	switch(s_2pCaliStep) {
		/* 3.1.  idle, can choose to enter the 2 point calibrate ---------------------------- */
		case kCs2Lclick: {
			g_sreg.btnLed[kBLed] = kLedNone; /* TBD */
			g_sreg.btnLed[kYLed] = kLedNone;
			g_sreg.btnLed[kRLed] = kLedNone;

			if (s_longClick == ACT) {
				if (s_longClickCnt > LONG_CLICK) {
					s_2pCaliStep = kCs2WaitRelease;
				}
			}
		}
		break;

		/* 3.2.  release longclick to savedist1 or getdistErr ------------------------------- */
		case kCs2WaitRelease: {
			g_sreg.btnLed[kYLed] = kLedToggle; /* TBD */
			g_sreg.btnLed[kBLed] = kLedToggle;
			g_sreg.btnLed[kRLed] = kLedOn;
			if (s_longClick == INA) {
				/* get cali2P2 */
				if (DistLmtChk() == -1) {
					s_2pCaliStep = kCs2Err;
				} else {
					g_reg[kRegCali2P2] = g_sreg.distance;
					s_2pCaliStep = kCs2Click;
				}
			}
		}
		break;

		/* 3.3.  singleclick again to savedist2 or getdist2Err ------------------------------ */
		case kCs2Click: {
			g_sreg.btnLed[kYLed] = kLedToggle;
			g_sreg.btnLed[kBLed] = kLedOff;
			g_sreg.btnLed[kRLed] = kLedOff;

			if (singleClick == ACT) {
				/* get and save cali1P2, cali2P2 */
				if (DistLmtChk() == -1) {
					s_2pCaliStep = kCs2Err;
				} else {
					s_2pCaliStep = kCs2LedTw2s;
					if (g_reg[kRegCali2P2] > g_sreg.distance) {
						g_reg[kRegCali1P2] = g_reg[kRegCali2P2];
						g_reg[kRegCali2P2] = g_sreg.distance;
					} else {
						g_reg[kRegCali1P2] = g_sreg.distance; /* kRegCali1P2  >  kRegCali2P2 */
					}
					g_reg[kReg1or2P] = 1; /* judge as 2 point in OutputTask */
					SetFlag(kFlagWriteReg);
				}
			}
		}
		break;

		/* 3.4.  call func CaliOkHandler and return idle ------------------------------------ */
		case kCs2LedTw2s: {
			if (CaliOkHandler() == 0) {
				s_2pCaliStep = kCs2Lclick;
			}
		}
		break;

		/* 3.5.  call func CaliErrHandler, singleclick to return idle ----------------------- */
		case kCs2Err: {
			CaliErrHandler();

			if (singleClick == ACT) {
				s_2pCaliStep = kCs2Lclick;
				return;
			}
		}
		break;
	}
	/* -------------------------------------------------------------------------------------- */
	if (s_2pCaliStep != kCs2Lclick) { /* 2 point calibrate running, TBD */
		if (s_1pCaliStep != kCs1Click1) {
			s_1pCaliStep = kCs1Click1;
		}
		return;
	}
	/* -------------------------------------------------------------------------------------- */
	/* 4.  one point calibrate interface ---------------------------------------------------- */
	static __IO uint32_t s_1pCaliCnt = 0;
	static __IO int32_t s_1pCaliTmo = 3 * 100;

	switch(s_1pCaliStep) {
		/* 4.1.  idle, can choose to enter the 1 point calibrate ---------------------------- */
		case kCs1Click1: {
			g_sreg.btnLed[kBLed] = kLedNone;
			g_sreg.btnLed[kYLed] = kLedNone;
			g_sreg.btnLed[kRLed] = kLedNone;

			if (singleClick == ACT) {
				s_1pCaliStep = kCs1Click2;
				s_1pCaliTmo = 3 * 100;
			} else if (doubleClick == ACT) {
				g_sreg.btnLed[kBLed] = kLedOff;
				g_sreg.btnLed[kYLed] = kLedOff;
				g_sreg.btnLed[kRLed] = kLedOff;

				/* save cali1P1 */
				if (DistLmtChk() == -1) {
					s_1pCaliStep = kCs1Err;
				} else {
					s_1pCaliStep = kCs1LedTw2s;

					g_reg[kRegCali1P1] = g_sreg.distance;
					g_reg[kReg1or2P] = 0; /* judge as 1 point in OutputTask */
					SetFlag(kFlagWriteReg);
				}
			}
		}
		break;
		
		/* 4.2.  perhaps kCs1Click2 can be deleted, TBD ------------------------------------- */
		case kCs1Click2: {
			g_sreg.btnLed[kBLed] = kLedOff;
			g_sreg.btnLed[kYLed] = kLedOff;
			g_sreg.btnLed[kRLed] = kLedOff;

			if (--s_1pCaliTmo != 0) {
				if (singleClick == ACT) {
					/* save cali1P1 */
					if (DistLmtChk() == -1) {
						s_1pCaliStep = kCs1Err;
					} else {
						s_1pCaliStep = kCs1LedTw2s;

						g_reg[kRegCali1P1] = g_sreg.distance;
						g_reg[kReg1or2P] = 0;
						SetFlag(kFlagWriteReg);
					}
				}
			} else {
				s_1pCaliStep = kCs1Click1;
			}
		}
		break;

		/* 4.3.  call func CaliOkHandler and return idle ------------------------------------ */
		case kCs1LedTw2s: {
			if (CaliOkHandler() == 0) {
				s_1pCaliStep = kCs1Click1;
			}
		}
		break;
		
		/* 4.4.  call func CaliErrHandler, singleclick to return idle ----------------------- */
		case kCs1Err: {
			CaliErrHandler();

			if (singleClick == ACT) {
				s_1pCaliStep = kCs1Click1;
			}
		}
		break;
	}
}
