#include "main.h"

static bool s_workLedMode = LIGHT_ON; 

void LedTask(void)
{
    /* Yellow LED reflects measurement status */
    if (s_workLedMode == LIGHT_ON) {
		//printf("Light On\r\n");
        /* Light on when object present, off when no object */
        if (g_outputState == OBJECT_PRESENT) {
            LED_SetWork(kLedOn);
        } else {
            LED_SetWork(kLedOff);
        }
    } else {
		//printf("Dark On\r\n");
        /* Light off when object present, on when no object */
        if (g_outputState == OBJECT_PRESENT) {
            LED_SetWork(kLedOff);
        } else {
            LED_SetWork(kLedOn);
        }
    }
}

void LedTask_Init(void)
{
    /* Green LED: Power indicator, always on when powered */
    LED_SetPwr(kLedOn);
    /* Yellow LED: default off */
    LED_SetWork(kLedOff);
	/* Use LED mode saved in Flash */
	LED_SetMode(ParamCfg_GetLedMode());
}

void LED_SetPwr(led_state_t state)
{
    if (state == kLedOn) {
        LED_Set(kLedGreen);
    } else {
        LED_Clr(kLedGreen);
    }
}

void LED_SetWork(led_state_t state)
{
    if (state == kLedOn) {
        LED_Set(kLedYellow);
    } else {
        LED_Clr(kLedYellow);
    }
}

void LED_SetMode(led_mode_t mode)
{
    s_workLedMode = mode;
}

led_mode_t LED_GetMode(void)
{
    return s_workLedMode;
}

