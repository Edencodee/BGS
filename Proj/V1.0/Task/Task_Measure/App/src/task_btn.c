#include "task_btn.h"
#include "bsp_gpio.h"
#include "task_led.h"

/* ==================== 内部状态 ==================== */
static bool s_led_mode = LIGHT_ON; /* 1: 有测量亮，无测量灭；0: 反转 */

static btn_state_t s_raw_state = BTN_RELEASED;
static btn_state_t s_stable_state = BTN_RELEASED;
static uint16_t s_debounce_cnt = 0U;

/* ==================== Button 任务 API ==================== */

void BtnTask_Init(void)
{
	s_led_mode = LIGHT_ON;
	s_raw_state = BtnGetState();
	s_stable_state = s_raw_state;
	s_debounce_cnt = 0U;

	/* 上电默认模式同步到 LED 任务 */
	Led_SetMode(s_led_mode);
}

bool Btn_GetLedMode(void)
{
	return s_led_mode;
}


void BtnTask(void)
{
	btn_state_t cur = BtnGetState();

	if (cur == s_raw_state) {
		if (s_debounce_cnt < BTN_DEBOUNCE_TICKS) {
			s_debounce_cnt++;
		}
	} else {
		s_raw_state = cur;
		s_debounce_cnt = 0U;
	}

	if (s_debounce_cnt >= BTN_DEBOUNCE_TICKS) {
		if (s_stable_state != s_raw_state) {
			s_stable_state = s_raw_state;

			/* 检测按下沿，切换黄灯模式 */
			if (s_stable_state == BTN_PRESSED) {
				s_led_mode = (s_led_mode == LIGHT_ON) ? DARK_ON : LIGHT_ON;
				Led_SetMode(s_led_mode);
			}
		}
	}
}