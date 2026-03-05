#include "main.h"

#define SHORT_TIMEOUT		3		/* 600us	*/
#define SHORT_RELEASE		10000	/* 2s		*/

__IO bool g_isShort = false;

void ShortChkTask (void)
{
	static __IO int32_t s_shortCnt = 0;

	if (g_isShort == false) {
		bool isNpn = (g_reg[kRegNPN] == kNpn);
		if (((isNpn) && (RESET == GetNpn(kCh1))) || ((!isNpn) && (SET == GetPnp(kCh1)))) {
			/* shut down output after short circuit 600us	*/
			if (LoopCntInc((int32_t * )&s_shortCnt, SHORT_TIMEOUT)) {
				isNpn ? NpnClr(kCh1) : PnpClr(kCh1); /* replace action in OutputTask to protect immediately */
				g_isShort = true;
				wlog("short");
			}
		} else {
			s_shortCnt = 0;
		}
	} else {
		/* recover after remove short circuit 2S  */
		if (LoopCntInc((int32_t * )&s_shortCnt, SHORT_RELEASE)) {
			g_isShort = false;
			wlog("short release");
		}
	}
}
