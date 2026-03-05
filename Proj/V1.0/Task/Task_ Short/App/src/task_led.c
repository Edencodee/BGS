#include "task_led.h"
#include "bsp_gpio.h"

/* ==================== 内部状态 ==================== */
static bool s_meas_active = false;  /* 测量状态指示（黄灯） */
static bool s_YLedMode = LIGHT_ON; /* 1: 有测量亮，无测量灭；0: 反转 */

/* ==================== LED 任务 API ==================== */

void LedTask_Init(void)
{
    /* 绿灯：电源指示，上电常亮 */
    Led_SetPwr(true);

    /* 黄灯：默认关闭 */
    Led_SetWork(false);

    s_meas_active = false;
    s_YLedMode = LIGHT_ON;
}

void Led_SetPwr(bool on)
{
    if (on) {
        LedSet(kLedGreen);
    } else {
        LedClr(kLedGreen);
    }
}

void Led_SetWork(bool on)
{
    if (on) {
        LedSet(kLedYellow);
    } else {
        LedClr(kLedYellow);
    }
}

void Led_SetMeasState(bool active)
{
    s_meas_active = active;
}

void Led_SetMode(bool light_on)
{
    s_YLedMode = light_on ? LIGHT_ON : DARK_ON;
}


void LedTask(void)
{
    /* 黄灯反映测量状态 */
    bool yellow_on = s_YLedMode ? s_meas_active : !s_meas_active;
    Led_SetWork(yellow_on);
}