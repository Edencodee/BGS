#include "main.h"

static __IO int32_t s_outState = 0;

static void JudgeTask(void)
{
	switch(g_reg[kRegState]) {
	case kLonBgs:
	case kDonBgs: {
		if (g_reg[kReg1or2P] == 0) { /* 1 point calibrate */
			int32_t dst;
			if (s_outState) {
				const int temp = g_reg[kRegCali1P1] * NOISE_RATIO;
				const int offs = (temp < BG_NOISE)? BG_NOISE : temp;
				dst = g_reg[kRegCali1P1] + offs;
			} else {
				dst = g_reg[kRegCali1P1];
			}
//			dst = (g_status > 0) ? dst + 1000 : dst;
			(g_sreg.distance < dst) ?  \
			ClrFlag(kFlagOutState) : SetFlag(kFlagOutState);
		} else { /* 2 point calibrate */
			int32_t start, stop;
			if (s_outState) {
				const int temp = g_reg[kRegCali2P2] * NOISE_RATIO;
				const int offs = (temp < BG_NOISE)? BG_NOISE : temp;
				start = g_reg[kRegCali2P2] - offs;

				const int temp1 = g_reg[kRegCali1P2] * NOISE_RATIO;
				const int offs1 = (temp1 < BG_NOISE)? BG_NOISE : temp1;
				stop = g_reg[kRegCali1P2] + offs1;
			} else {
				start = g_reg[kRegCali2P2];
				stop  = g_reg[kRegCali1P2];
			}
//			if (g_status > 0) {
//				start -= 1000;
//				stop  += 1000;
//			}
			(g_sreg.distance > start) && (g_sreg.distance < stop) ? \
			ClrFlag(kFlagOutState) : SetFlag(kFlagOutState);
		}
	}
	break;

	case kLonFgs:
	case kDonFgs: { /* TBD */
		if (g_reg[kReg1or2P] == 0) { /* 1 point calibrate */
			int32_t dst;
			const int temp = g_reg[kRegCali1P1] * NOISE_RATIO;
			if (s_outState) {
				const int offs = (temp < BG_NOISE)? BG_NOISE : temp;
				dst = g_reg[kRegCali1P1] - offs;							
			} else {
				const int offs = (temp < BG_NOISE)? (BG_NOISE << 1) : (temp << 1);
				dst = g_reg[kRegCali1P1] - offs;				
			}
//			dst = (g_status > 0) ? dst - 1000 : dst;
			(g_sreg.distance < dst) ?  \
			ClrFlag(kFlagOutState) : SetFlag(kFlagOutState);
		} else { /* 2 point calibrate */
			int32_t start, stop;
			const int temp  = g_reg[kRegCali2P2] * NOISE_RATIO;
			const int temp1 = g_reg[kRegCali1P2] * NOISE_RATIO;
			if (s_outState) {
				const int offs = (temp < BG_NOISE)? BG_NOISE : temp;
				start = g_reg[kRegCali2P2] - offs;
				const int offs1 = (temp1 < BG_NOISE)? BG_NOISE : temp1;
				stop = g_reg[kRegCali1P2] - offs1;
			} else {
				start = g_reg[kRegCali2P2];
				const int offs = (temp < BG_NOISE)? (BG_NOISE << 1) : (temp << 1);
				stop  = g_reg[kRegCali1P2] - offs;
			}
//			if (g_status > 0) {
//				start -= 1000;
//				stop  += 1000;
//			}
			(g_sreg.distance > start) && (g_sreg.distance < stop) ? \
			ClrFlag(kFlagOutState) : SetFlag(kFlagOutState);
		}
	}
	break;
	}

	s_outState = ChkFlag(kFlagOutState) ? 1 : 0;
}

void OutputTask (void)
{
	int32_t pol = (g_reg[kRegState] == kLonBgs) || (g_reg[kRegState] == kLonFgs); /* light on = 1 / dark on = 0 */

	/* 1. set g_outState -------------------------------------------------------------------- */
	JudgeTask();
	/* -------------------------------------------------------------------------------------- */
	/* 2. set/clr npn/pnp ------------------------------------------------------------------- */
	if (ChkFlag(kFlagShort)) {
//		isNpn ? NpnClr(kCh1) : PnpClr(kCh1); // replaced by rapid action in ShortChkTask
		s_outState = -1; /* reset to INA state */
	} else {
		if (pol) { /* L -on */
			if (s_outState) {
				NpnClr(kCh1);
				PnpClr(kCh1);
			} else {
				NpnSet(kCh1);
				PnpSet(kCh1);
			}
		} else { /* D -on */
			if (s_outState) {
				NpnSet(kCh1);
				PnpSet(kCh1);
			} else {
				NpnClr(kCh1);
				PnpClr(kCh1);
			}
		}
	}
}
