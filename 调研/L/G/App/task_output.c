#include "main.h"

#define BLIND_ZONE			20		/* 盲区距离阈值 */

static __IO int32_t s_outState = 0; /* 当前输出状态 */
static __IO int32_t bg_suppression_value     = 0; /* 单点校准的阈值点 */
static __IO int32_t detection_threshold     = 0; /* 单点校准的阈值点 */

#define BG_SUPPRESSION_RATIO  30   // 背景抑制百分比
#define NOISE_THRESHOLD       50   // 噪声阈值
static void JudgeTask(void)
{
	if (g_reg[kReg1or2P] == 0) {
        int primary_threshold = g_reg[kRegCali1P1];
        
        /* 计算背景抑制值 */
        bg_suppression_value = (primary_threshold * BG_SUPPRESSION_RATIO) / 100;
        if (bg_suppression_value > NOISE_THRESHOLD) {
            bg_suppression_value = NOISE_THRESHOLD;
        }
        
        /* 应用背景抑制：检测时使用更高阈值避免背景干扰 */
        if (s_outState == 0) {
            /* 初始检测时要求更严格 */
            detection_threshold = primary_threshold - bg_suppression_value;
        } else {
            /* 已检测到物体后，保持检测直到物体明显离开 */
            detection_threshold = primary_threshold + bg_suppression_value;
        }
        
        /* 判断逻辑 */
				(g_sreg.distance < detection_threshold) ? \
		ClrFlag(kFlagOutState) : SetFlag(kFlagOutState);
    }
    
   s_outState = ChkFlag(kFlagOutState) ? 1 : 0; 
}

void OutputTask (void)
{
	int32_t pol = (g_reg[kRegState] == kLonBgs); /* 判断当前是亮触发还是暗触发，亮触发=1，暗触发=0 */

	/* 1. 设置g_outState -------------------------------------------------------------------- */
	JudgeTask(); /* 调用JudgeTask函数判断输出状态 */
	static int32_t s_1000msCnt = 0; /* 1000ms计数器 */
	if (LoopCntInc(&s_1000msCnt, 10)) { /* 每1000ms执行一次 */
//		printf("SPointA = %d, CurRange = %d \r\n", SPoint, g_sreg.distance); /* 单点校准调试信息 */
//		printf("DPointA = %d, DPointB = %d, CurRange = %d \r\n", DPointA, DPointB, g_sreg.distance); /* 双点校准调试信息 */
//		printf("\r\n");
	}
	/* -------------------------------------------------------------------------------------- */
	/* 2. 设置/清除npn/pnp ------------------------------------------------------------------- */
	if (ChkFlag(kFlagShort)) {
//		isNpn ? NpnClr(kCh1) : PnpClr(kCh1); // 短路检测时的操作，已替换为ShortChkTask中的快速动作
		s_outState = -1; /* 重置为INA状态 */
	} else if (ChkFlag(kFlagCloseOut)) {
		NpnClr(kCh1); /* 清除npn输出 */
		PnpClr(kCh1); /* 清除pnp输出 */
	} else {
		/* 根据盲区和输出状态设置npn/pnp输出 */
		if (g_sreg.distance <= BLIND_ZONE) { /* 如果距离在盲区内 */
			if (pol) { /* 亮触发 */
				NpnClr(kCh1); /* 清除npn输出 */
				PnpClr(kCh1); /* 清除pnp输出 */
			} else { /* 暗触发 */
				NpnSet(kCh1); /* 设置npn输出 */
				PnpSet(kCh1); /* 设置pnp输出 */
			}
		} else {
			if (pol) { /* 亮触发 */
				if (s_outState) { /* 如果当前输出状态为1 */
					NpnClr(kCh1); /* 清除npn输出 */
					PnpClr(kCh1); /* 清除pnp输出 */
				} else {
					NpnSet(kCh1); /* 设置npn输出 */
					PnpSet(kCh1); /* 设置pnp输出 */
				}
			} else { /* 暗触发 */
				if (s_outState) { /* 如果当前输出状态为1 */
					NpnSet(kCh1); /* 设置npn输出 */
					PnpSet(kCh1); /* 设置pnp输出 */
				} else {
					NpnClr(kCh1); /* 清除npn输出 */
					PnpClr(kCh1); /* 清除pnp输出 */
				}
			}
		}
	}
}