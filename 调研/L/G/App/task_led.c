#include "main.h"

#define TWK_FAST			200 // 定义快速闪烁的时间间隔
#define TWK_SLOW			500 // 定义慢速闪烁的时间间隔
#define BLIND_ZONE			20 	// 定义盲区距离阈值

void LedTask (void)
{
	// 静态变量，用于保存LED状态和闪烁计数器等信息
	static __IO led_state_t s_ledState[2] = {kLedOff}; // 保存两个LED的状态
	static __IO int32_t s_toggleCnt = 0; // 闪烁计数器
	static __IO int32_t s_isLedOn[2] = {0}; // 记录每个LED是否处于开启状态
	static int32_t s_toggleMax = TWK_SLOW; // 当前闪烁周期的最大值，默认为慢速

	/* 1. 根据系统状态和按钮输入设置LED状态 ----------------------------------------------- */
	switch(g_reg[kRegState]) {
		case kLonBgs: { // 当系统状态为kLonBgs时，两个LED都打开
			s_ledState[kGLed] = kLedOn;
			s_ledState[kYLed] = kLedOn;
		}
		break;
		case kDonBgs: { // 当系统状态为kDonBgs时，绿灯打开，黄灯关闭
			s_ledState[kGLed] = kLedOn;
			s_ledState[kYLed] = kLedOff;
		}
		break;
		default:
		; // 其他状态不做处理
	}
	
	// 如果按钮输入不为空，则根据按钮输入覆盖LED状态
	if (g_sreg.btnLed[kGLed] != kLedNone) {
		s_ledState[kGLed] = (led_state_t)g_sreg.btnLed[kGLed];
	}

	if (g_sreg.btnLed[kYLed] != kLedNone) {
		s_ledState[kYLed] = (led_state_t)g_sreg.btnLed[kYLed];
	}

	// 设置闪烁周期，默认为慢速
	s_toggleMax = TWK_SLOW;
	if (ChkFlag(kFlagShort)) { // 如果设置了kFlagShort标志，则快速闪烁黄灯
		s_ledState[kGLed] = kLedToggle;
		s_toggleMax = TWK_FAST;
	} else if (g_sreg.btnLed[kGLed] != kLedNone) {
		// 如果按钮输入不为空，则根据按钮输入设置绿灯状态
		s_ledState[kGLed] = (led_state_t)g_sreg.btnLed[kGLed];
	} else {
		/* 根据距离和系统状态控制绿灯状态 */
		if (g_sreg.distance <= BLIND_ZONE) { // 如果距离小于等于盲区阈值
			if (g_reg[kRegState] == kLonBgs){ // 如果系统状态为kLonBgs，则关闭绿灯
				s_ledState[kGLed] = kLedOff;
			} else { // 否则打开绿灯
				s_ledState[kGLed] = kLedOn;
			}
		} else {
			if (g_reg[kRegState] == kLonBgs){ // 如果系统状态为kLonBgs
				s_ledState[kGLed] = ChkFlag(kFlagOutState) ? kLedOff : kLedOn; // 根据kFlagOutState标志切换绿灯状态
			} else { // 否则
				s_ledState[kGLed] = ChkFlag(kFlagOutState) ? kLedOn : kLedOff; // 根据kFlagOutState标志切换绿灯状态
			}
		}
	}


	if ( (s_ledState[kGLed] == kLedToggle) || \
	    (s_ledState[kYLed] == kLedToggle) || \
	    (s_ledState[kGLed] == kLedToggle)) {
		s_toggleCnt = (s_toggleCnt + 1) % s_toggleMax;
	} else {
		s_toggleCnt = 0; // 如果没有LED闪烁，则重置计数器
	}
	/* -------------------------------------------------------------------------------------------- */

	/* 主LED控制循环 --------------------------------------------------------------------------- */
	for(int i = 0; i < LED_N; i++) { // 遍历所有LED
		switch(s_ledState[i]) { // 根据每个LED的状态进行控制
		case kLedOn: { // 如果LED应处于开启状态
			if (s_isLedOn[i] != 1) { // 如果当前未开启
				LedSet((led_t)i); // 打开LED
				s_isLedOn[i] = 1; // 更新状态记录
			}
		}
		break;

		case kLedOff: { // 如果LED应处于关闭状态
			if (s_isLedOn[i] != 0) { // 如果当前未关闭
				LedClr((led_t)i); // 关闭LED
				s_isLedOn[i] = 0; // 更新状态记录
			}
		}
		break;

		case kLedToggle: { // 如果LED应处于闪烁状态
			if (s_toggleCnt < (s_toggleMax / 2)) { // 在闪烁周期的前半部分
				if (s_isLedOn[i] != 0) { // 如果当前处于开启状态
					LedClr((led_t)i); // 关闭LED
					s_isLedOn[i] = 0; // 更新状态记录
				}
			} else { // 在闪烁周期的后半部分
				if (s_isLedOn[i] != 1) { // 如果当前处于关闭状态
					LedSet((led_t)i); // 打开LED
					s_isLedOn[i] = 1; // 更新状态记录
				}
			}
		}
		break;

		default:
			; // 其他状态不处理
		}
	}
}