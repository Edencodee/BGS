#include "main.h"

static btn_state_t s_raw_state = BTN_RELEASED;
static btn_state_t s_stable_state = BTN_RELEASED;
static uint16_t s_debounce_cnt = 0U;
static void Btn_ModeToggle(void);


void BtnTask(void)
{
	btn_state_t cur_state = Btn_GetState();

	if (cur_state == s_raw_state) {
		if (s_debounce_cnt < BTN_DEBOUNCE_TICKS) {
			s_debounce_cnt++;
		}
	} else {
		s_raw_state = cur_state;
		s_debounce_cnt = 0U;
	}

	if (s_debounce_cnt >= BTN_DEBOUNCE_TICKS) {
		if (s_stable_state != s_raw_state) {
			s_stable_state = s_raw_state;

			if (s_stable_state == BTN_PRESSED) {
				Btn_ModeToggle();
			}
		}
	}
}

void BtnTask_Init(void)
{
	s_raw_state = Btn_GetState();
	s_stable_state = s_raw_state;
	s_debounce_cnt = 0U;
}


static void Btn_ModeToggle(void)
{
	led_mode_t cur_mode = (led_mode_t)g_param_cfg.app.led_mode;
	led_mode_t next_mode = (cur_mode == LIGHT_ON) ? DARK_ON : LIGHT_ON;

	/* Update parameter and save to Flash */
	g_param_cfg.app.led_mode = (uint8_t)next_mode;
	//Param_Save();
    
	printf("LED_Mode: %s\r\n", (next_mode == LIGHT_ON) ? "Light On" : "Dark On");
}

btn_state_t Btn_GetState (void)
{
	
	if (gpio_input_bit_get(BTN_PORT, BTN_PIN) == RESET) {
        return BTN_PRESSED;
    } else {
        return BTN_RELEASED;
    }
}