#include "main.h"

void LedTask(void)
{
    /* Short circuit protection: force LED and NPN off */
    if (g_outputState == SHORT_CIRCUIT) {
        LED_SetWork(kLedOff);  /* LED_SetWork already calls NPN_Clr() */
        return;
    }
    
    /* Use LED mode from Flash parameter */
    led_mode_t led_mode = (led_mode_t)g_param_cfg.app.led_mode;
    
    /* Yellow LED reflects measurement status */
    if (led_mode == LIGHT_ON) {
        /* Light on when object present, off when no object */
        if (g_outputState == OBJECT_PRESENT) {
            LED_SetWork(kLedOn);
        } else {
            LED_SetWork(kLedOff);
        }
    } else {
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
    //LED_SetPwr(kLedOn);
    /* Yellow LED: initial state based on Flash parameter */
    LED_SetWork(kLedOff);
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
        //LED_Set(kLedYellow);
        NPN_Set();  /* NPN output follows LED state */
    } else {
        //LED_Clr(kLedYellow);
        NPN_Clr();  /* NPN output follows LED state */
    }
}

