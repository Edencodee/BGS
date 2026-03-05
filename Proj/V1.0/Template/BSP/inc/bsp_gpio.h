#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H


#ifdef cplusplus
extern "C" {
#endif

#include "main.h"
#include "gd32c2x1.h"

/* led IO --------------------------------------------- */
#define LED_N               	1U
#define LED1_PIN           		GPIO_PIN_8	
#define LED1_PORT     			GPIOB
#define LED1_CLK      			RCU_GPIOB
/* ---------------------------------------------------- */

/* led typedef ---------------------------------------- */
typedef enum {
	kLed1 = 0,
	kLed2
} led_t;

typedef enum {
	kLedOff = 0,
	kLedOn,
	kLedToggle,
	kLedNone
} led_state_t;
/* ---------------------------------------------------- */

/* function declaration ------------------------------- */
void LedClr(led_t led);
void LedSet(led_t led);
void GPIO_UserInit(void);
/* ---------------------------------------------------- */



#ifdef cplusplus
}
#endif

#endif
