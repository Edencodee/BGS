#ifndef __GPIO_H
#define __GPIO_H

#ifdef cplusplus
extern "C" {
#endif

#include "n32l40x.h"

#define BS_LONG_TICKS      		1000    	/* 1000ms 		*/
#define BS_SHORT_TMO   			400    		/* 400ms not_usd double click	*/

#define INA						-1
#define ACT						0

/* key IO --------------------------------------------- */
#define PB_N                   	1U
#define PB1_PIN           		GPIO_PIN_14
#define PB1_PORT     			GPIOD
#define PB1_CLK      			RCC_APB2_PERIPH_GPIOD
/* ---------------------------------------------------- */

/* output IO ------------------------------------------ */
#define OUT_N					1U
#define NPN1_PIN           		GPIO_PIN_6	/* NPN		*/
#define NPN1_PORT     			GPIOA
#define NPN1_CLK      			RCC_APB2_PERIPH_GPIOA
#define FUN1_PIN           		GPIO_PIN_7	/* FUN		*/
#define FUN1_PORT     			GPIOA
#define FUN1_CLK      			RCC_APB2_PERIPH_GPIOA
#define PNP1_PIN           		GPIO_PIN_1	/* PNP		*/
#define PNP1_PORT     			GPIOA
#define PNP1_CLK      			RCC_APB2_PERIPH_GPIOA
#define FUP1_PIN           		GPIO_PIN_2	/* FUP		*/
#define FUP1_PORT     			GPIOA
#define FUP1_CLK      			RCC_APB2_PERIPH_GPIOA
/* ---------------------------------------------------- */

/* LED IO --------------------------------------------- */
#define LED_N					3U
#define LEDR_PIN           		GPIO_PIN_6
#define LEDR_PORT     			GPIOB
#define LEDR_CLK      			RCC_APB2_PERIPH_GPIOB
#define LEDG_PIN           		GPIO_PIN_4
#define LEDG_PORT     			GPIOB
#define LEDG_CLK      			RCC_APB2_PERIPH_GPIOB
#define LEDY_PIN           		GPIO_PIN_15
#define LEDY_PORT     			GPIOA
#define LEDY_CLK      			RCC_APB2_PERIPH_GPIOA
/* ---------------------------------------------------- */

/* SW IO ---------------------------------------------- */
#define SW_PIN					GPIO_PIN_11
#define SW_PORT					GPIOA
#define SW_CLK      			RCC_APB2_PERIPH_GPIOA
/* ---------------------------------------------------- */

/* TOF sensor IO define ------------------------------- */
#define TOF_N					2U	
#define XSHUT_PIN				GPIO_PIN_5
#define XSHUT_PORT				GPIOA
#define XSHUT_CLK      			RCC_APB2_PERIPH_GPIOA	
#define GPIO1_PIN				GPIO_PIN_0
#define GPIO1_PORT				GPIOB
#define GPIO1_CLK      			RCC_APB2_PERIPH_GPIOB
/* ---------------------------------------------------- */
	/* key press state typedef */
	typedef enum {
		kNoneClick = 0,
		kSingleClick,
		kDoubleClick,
		kLongClick,
		kLongClickRelease
	} key_press_state_t;

	/* channel typedef */
	typedef enum {
		kCh1 = 0,
	} ch_t;

	/* switch typedef */
	typedef enum {
		kPb1 = 0,
	} pb_t;
	
	/* led typedef */
	typedef enum {
		kRLed = 0,	/* RED1,    TBD */
		kBLed,		/* BLUE2,   TBD */
		kYLed		/* YELLOW3, TBD */
	} led_t;
	
	typedef enum {
		kLedOff = 0,
		kLedOn,
		kLedToggle,
		kLedNone
	} led_state_t;

	/* function declaration --------------------------- */
	void NpnClr(ch_t ch);
	void NpnSet(ch_t ch);
	void PnpClr(ch_t ch);
	void PnpSet(ch_t ch);
	void LedClr(led_t led);
	void LedSet(led_t led);
	uint8_t GetNpn(ch_t ch);
	uint8_t GetPnp(ch_t ch);
	uint8_t GetPb(pb_t pb);
	void XshutClr(void);
	void XshutSet(void);
	void SwClr(void);
	void SwSet(void);
	uint8_t GetGPIO1(void);
	void GPIO_UserInit(void);
	void GPIO_PbScan(uint8_t index);
	key_press_state_t GPIO_ReadPbStatus(uint8_t index);
	/* ------------------------------------------------ */

#ifdef cplusplus
}
#endif

#endif
