#include "main.h"

#define TWK_FAST			200
#define TWK_SLOW			500

void LedTask (void)
{
	static __IO led_state_t s_ledState[3] = {kLedOff};
	static __IO int32_t s_toggleCnt = 0;
	static __IO int32_t s_isLedOn[3] = {0};
	static int32_t s_toggleMax = TWK_SLOW;

	/* 1.  led state setting ---------------------------------------------------------------- */
	switch(g_reg[kRegState]) {
		case kLonBgs: {
			s_ledState[kBLed] = kLedOn;
			s_ledState[kYLed] = kLedOn;
		}
		break;
		case kDonFgs: {
			s_ledState[kBLed] = kLedOff;
			s_ledState[kYLed] = kLedOff;
		}
		break;
		case kLonFgs: {
			s_ledState[kBLed] = kLedOff;
			s_ledState[kYLed] = kLedOn;
		}
		break;
		case kDonBgs: {
			s_ledState[kBLed] = kLedOn;
			s_ledState[kYLed] = kLedOff;
		}
		break;
		default:
		;
	}
	
	if (g_sreg.btnLed[kBLed] != kLedNone) {
		s_ledState[kBLed] = (led_state_t)g_sreg.btnLed[kBLed];
	}

	if (g_sreg.btnLed[kYLed] != kLedNone) {
		s_ledState[kYLed] = (led_state_t)g_sreg.btnLed[kYLed];
	}

	s_toggleMax = TWK_SLOW;
	if (ChkFlag(kFlagShort)) { // Yellow
//		s_ledState[kBLed] = kLedOff;
//		s_ledState[kYLed] = kLedOff;
		s_ledState[kRLed] = kLedToggle;
		s_toggleMax = TWK_FAST;
	} else if (g_sreg.btnLed[kRLed] != kLedNone) {
		s_ledState[kRLed] = (led_state_t)g_sreg.btnLed[kRLed];
	} else {
		if ((g_reg[kRegState] == kLonBgs) || (g_reg[kRegState] == kLonFgs)) { /* L -on */
			s_ledState[kRLed] = ChkFlag(kFlagOutState) ? kLedOff : kLedOn; // (led_state_t)g_outState;
		} else { /* D -on */
			s_ledState[kRLed] = ChkFlag(kFlagOutState) ? kLedOn : kLedOff; // (led_state_t)(1 - g_outState);
		}
	}

	if ( (s_ledState[kBLed] == kLedToggle) || \
	    (s_ledState[kYLed] == kLedToggle) || \
	    (s_ledState[kRLed] == kLedToggle)) {
		s_toggleCnt = (s_toggleCnt + 1) % s_toggleMax;
	} else {
		s_toggleCnt = 0;
	}
	/* ----------------------------------------------------------------------------------------------------- */

	/* led routine ----------------------------------------------------------------------------------------- */
	for(int i = 0; i < LED_N; i++) {
		switch(s_ledState[i]) {
		case kLedOn: {
			if (s_isLedOn[i] != 1) {
				LedSet((led_t)i);
				s_isLedOn[i] = 1;
			}
		}
		break;

		case kLedOff: {
			if (s_isLedOn[i] != 0) {
				LedClr((led_t)i);
				s_isLedOn[i] = 0;
			}
		}
		break;

		case kLedToggle: {
			if (s_toggleCnt < (s_toggleMax / 2)) {
				if (s_isLedOn[i] != 0) {
					LedClr((led_t)i);
					s_isLedOn[i] = 0;
				}
			} else {
				if (s_isLedOn[i] != 1) {
					LedSet((led_t)i);
					s_isLedOn[i] = 1;
				}
			}
		}
		break;

		default:
			;
		}
	}
}
