#include "main.h"

__IO int32_t g_outState = 0;

bool LoopCntIncU16(uint16_t *cnt, uint16_t max)
{
	if (++(*cnt) >= max) {
		*cnt = 0;
		return true;
	}
	return false;
}

void OutputTask (void)
{
	static __IO int32_t s_outState = 0;

	/* 1. NONC1 -> NPN ch, NONC2 -> PNP ch -------------------------------------------------- */
	bool isNpn = (g_reg[kRegNPN] == kNpn);
	int group = isNpn ? 0 : 1;
	int32_t pol = isNpn ? g_reg[kRegNONC1] : g_reg[kRegNONC2];
	isNpn ? PnpClr(kCh1) : NpnClr(kCh1); // turn off ch not selected
	/* -------------------------------------------------------------------------------------- */
	/* 2. set g_outState -------------------------------------------------------------------- */
	if (ChkFlag(kFlagOverRange)) {
		g_outState = 0;
	} else {
		int32_t dist = g_sreg.dist10 / 10.0f;
		int32_t diff = g_sreg.diff;
		int32_t tolerance = g_sreg.tolerance;
		switch (g_reg[kRegOut]) {
		case kGenOut: {
			int32_t thr = getCurrThre(group);
			if (s_outState == 0) {
				if (dist < (thr - diff)) {
					g_outState = 1;
				}
			} else {
				if (dist > (thr + diff)) {
					g_outState = 0;
				}
			}
		}
		break;

		case k1PTeach: {
			int32_t thr1 = getThre(group, 0);
			int32_t thr2 = getThre(group, 1);
			if (thr1 > thr2) {
				int temp = thr1;
				thr1 = thr2;
				thr2 = temp;
			}
			if (g_outState == 0) {
				if ((dist < thr2 - diff) && (dist > thr1 + diff)) {
					g_outState = 1;
				}
			} else {
					if ((dist > thr2 ) || (dist < thr1 )) {
						g_outState = 0;
					}
			}
		}
		break;

		case k2PTeach:
		case k3PTeach: {
			int32_t thr1 = getThre(group, 0);
			int32_t thr2 = getThre(group, 1);
			if (thr1 > thr2) {
				int temp = thr1;
				thr1 = thr2;
				thr2 = temp;
			}
			if (g_outState == 0) {
				if ((dist < thr2) && (dist > thr1)) {
					g_outState = 1;
				}
			} else {
				if ((dist > thr2 + diff) || (dist < thr1 - diff)) {
					g_outState = 0;
				}
			}
		}
		break;
		}
	}
	/* -------------------------------------------------------------------------------------- */
	/* 3. output delay ---------------------------------------------------------------------- */
	int32_t outState = s_outState;
	if (g_reg[kRegPeriod] == 0) {
		return;
	}
	uint16_t times = 1000 / g_reg[kRegPeriod]; // dlyT unit: ms
	uint16_t dlyT = g_reg[kRegDlyT] * times;
	static uint16_t dlyCnt = 0;
	if (g_outState != s_outState) {
		switch (g_reg[kRegDly]) {
		case kDlyOff: {
			outState = g_outState;
		}
		break;
		case kOnDly: {
			if (g_outState) {
				if (LoopCntIncU16(&dlyCnt, dlyT)) {
					outState = g_outState;
				} /* else outState = s_outState */
			} else {
				dlyCnt = 0;
				outState = g_outState; /* clear immediately */
			}
		}
		break;
		case kOffDly: {
			if (g_outState == 0) {
				if (LoopCntIncU16(&dlyCnt, dlyT)) {
					outState = g_outState;
				} /* else outState = s_outState */
			} else {
				dlyCnt = 0;
				outState = g_outState; /* set immediately */
			}
		}
		break;
		case kOnOffDly: {
			if (LoopCntIncU16(&dlyCnt, dlyT)) {
				outState = g_outState;
			} /* else outState = s_outState */
		}
		break;
		case kSinglePulse: {
			if (s_outState) {
				if (LoopCntIncU16(&dlyCnt, dlyT)) {
					outState = 0; /* single pulse contrl */
				} /* else outState = s_outState */
			} else {
				dlyCnt = 0;
				outState = g_outState; /* set immediately */
			}
		}
		break;
		}
	} else {
		/* outState = s_outState; */
		dlyCnt = 0;
	}
	/* ------------------------------------------------------------------------------------- */
	/* 4. set/clr npn/pnp ------------------------------------------------------------------ */
	if (g_isShort) {
//		isNpn ? NpnClr(kCh1) : PnpClr(kCh1); // replaced by rapid action in ShortChkTask
		s_outState = -1; /* reset to INA state */
	} else if (outState) {
		if (s_outState != 1) {
			if (pol == kNO) {
				isNpn ? NpnSet(kCh1) : PnpSet(kCh1);
			} else {
				isNpn ? NpnClr(kCh1) : PnpClr(kCh1);
			}
			s_outState = 1;
		}
	} else {
		if (s_outState != 0) {
			if (pol == kNO) {
				isNpn ? NpnClr(kCh1) : PnpClr(kCh1);
			} else {
				isNpn ? NpnSet(kCh1) : PnpSet(kCh1);
			}
			s_outState = 0;
		}
	}
	/* -------------------------------------------------------------------------------------- */
}
