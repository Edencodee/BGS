#include "main.h"

#define SHORT_TIMEOUT		3		/* 600us	*/
#define SHORT_RELEASE		10000	/* 2s		*/

void ShortChkTask (void)
{
	static __IO int32_t s_shortCnt = 0;

	if (!ChkFlag(kFlagShort)) {
		bool isNpn = (g_reg[kRegState] == kLonBgs) || (g_reg[kRegState] == kLonFgs);
		if (((isNpn) && (RESET == GetNpn(kCh1))) || ((!isNpn) && (SET == GetPnp(kCh1)))) {
			/* shut down output after short circuit 600us */
			if (LoopCntInc((int32_t * )&s_shortCnt, SHORT_TIMEOUT)) {
				isNpn ? NpnClr(kCh1) : PnpClr(kCh1); /* replace action in OutputTask to protect immediately */
				SetFlag(kFlagShort);
				wlog("short");
			}
		} else {
			s_shortCnt = 0;
		}
	} else {
		/* recover after remove short circuit 2S */
		if (LoopCntInc((int32_t * )&s_shortCnt, SHORT_RELEASE)) {
			ClrFlag(kFlagShort);
			wlog("short release");
		}
	}
}
