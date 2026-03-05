#include "main.h"

#define TWK_FAST			250
#define TWK_SLOW			500

void LedTask (void)
{
	static __IO led_state_t s_ledState[LED_N] = {kLedOff};
	static __IO int32_t s_toggleCnt = 0;
	static __IO bool s_isLedOn[LED_N] = {false};
	int32_t toggleMax = TWK_SLOW;

	/* 1. led state setting ---------------------------------------------------------- */
	int index = (g_reg[kRegNPN] == kNpn) ? 0 : 1;
	if (g_isShort) {
		s_ledState[index] = kLedToggle;
		toggleMax = TWK_FAST;
	} else {
		s_ledState[index] = (led_state_t)g_outState; // 0:LED_OFF 1:LED_ON
	}
	s_ledState[1 - index] = kLedOff;
	/* 2. twinkle control ------------------------------------------------------------ */
	for (int i = 0; i < LED_N; i++) {
		if (s_ledState[(led_t)i] == kLedToggle) {
			s_toggleCnt = (s_toggleCnt + 1) % toggleMax;
			break;
		}
	}
	/* 3. led routine ---------------------------------------------------------------- */
	for (int i = 0; i < LED_N; i++) {
		switch (s_ledState[i]) {
		case kLedOn: {
			if (!s_isLedOn[i]) {
				LedSet((led_t)i);
				s_isLedOn[i] = true;
			}
		}
		break;
		case kLedOff: {
			if (s_isLedOn[i]) {
				LedClr((led_t)i);
				s_isLedOn[i] = false;
			}
		}
		break;
		case kLedToggle: {
			if (s_toggleCnt < (toggleMax / 2)) {
				if (s_isLedOn[i]) {
					LedClr((led_t)i);
					s_isLedOn[i] = false;
				}
			} else {
				if (!s_isLedOn[i]) {
					LedSet((led_t)i);
					s_isLedOn[i] = true;
				}
			}
		}
		break;
		default:
			;
		}
	}
	/* -------------------------------------------------------------------------------------- */
}
