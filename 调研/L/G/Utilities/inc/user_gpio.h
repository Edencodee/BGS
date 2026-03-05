#ifndef __GPIO_H
#define __GPIO_H

#ifdef cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "gd32c2x1.h"

/* ld IO --------------------------------------------- */
#define LD_N                1U
#define LD1_PIN           		GPIO_PIN_4	/* BLUE		*/
#define LD1_PORT     				GPIOA
#define LD1_CLK      				RCU_GPIOA
/* ---------------------------------------------------- */

/* PD IO ------------------------------------------ */
#define PD_N					1U
#define STS_OUT1_PIN         GPIO_PIN_3	/* FUP		*/
#define STS_OUT1_PORT      	GPIOA
#define STS_OUT1_CLK       	RCU_GPIOA
#define CRC_RST1_PIN         GPIO_PIN_5	/* FUP		*/
#define CRC_RST1_PORT      	GPIOA
#define CRC_RST1_CLK       	RCU_GPIOA
#define PLS_SO1_PIN          GPIO_PIN_6	/* NPN		*/
#define PLS_SO1_PORT     		GPIOA
#define PLS_SO1_CLK      		RCU_GPIOA
#define CLK_MODE1_PIN            	GPIO_PIN_7	/* PNP		*/
#define CLK_MODE1_PORT      			GPIOA
#define CLK_MODE1_CLK       			RCU_GPIOA
#define CLOSE_PLS_MCU1_PIN           		GPIO_PIN_0	/* FUN		*/
#define CLOSE_PLS_MCU1_PORT     			GPIOB
#define CLOSE_PLS_MCU1_CLK      			RCU_GPIOB
#define FREQ_CTRL1_PIN           	GPIO_PIN_15	/* FUP		*/
#define FREQ_CTRL1_PORT      			GPIOA
#define FREQ_CTRL1_CLK       			RCU_GPIOA
#define Cal_CLK_PIN           	GPIO_PIN_3	/* FUP		*/
#define Cal_CLK_PORT      			GPIOB
#define Cal_CLK_CLK       			RCU_GPIOB
/* ---------------------------------------------------- */

/* led IO --------------------------------------------- */
#define LED_N                   2U
#define GLED_PIN           		GPIO_PIN_1	/* BLUE		*/
#define GLED_PORT     			GPIOB
#define GLED_CLK      			RCU_GPIOB
#define YLED_PIN            	GPIO_PIN_2	/* RED 		*/
#define YLED_PORT      			GPIOB
#define YLED_CLK       			RCU_GPIOB
/* ---------------------------------------------------- */

/* output IO ------------------------------------------ */
#define OUT_N					1U
#define NPN1_PIN           		GPIO_PIN_5	/* NPN		*/
#define NPN1_PORT     			GPIOB
#define NPN1_CLK      			RCU_GPIOB
#define PNP1_PIN            	GPIO_PIN_5	/* PNP		*/
#define PNP1_PORT      			GPIOB
#define PNP1_CLK       			RCU_GPIOB
#define FUN1_PIN           		GPIO_PIN_6	/* FUN		*/
#define FUN1_PORT     			GPIOB
#define FUN1_CLK      			RCU_GPIOB
#define FUP1_PIN            	GPIO_PIN_6	/* FUP		*/
#define FUP1_PORT      			GPIOB
#define FUP1_CLK       			RCU_GPIOB
/* ---------------------------------------------------- */


	/* led typedef ------------------------------------ */
	typedef enum {
		kGLed = 0,
		kYLed
	} led_t;

	typedef enum {
		kLedOff = 0,
		kLedOn,
		kLedToggle,
		kLedNone
	} led_state_t;
	/* ------------------------------------------------ */

	/* channel typedef */
	typedef enum {
		kCh1 = 0,
	} ch_t;

	/* function declaration --------------------------- */
	void NpnClr(ch_t ch);
	void NpnSet(ch_t ch);
	void PnpClr(ch_t ch);
	void PnpSet(ch_t ch);
	void LedClr(led_t led);
	void LedSet(led_t led);
	void LDClr (ch_t ch);
	void LDSet (ch_t ch);
	void CRCRSTClr (ch_t ch);
	void CRCRSTSet (ch_t ch);
	void CLKMODEClr (ch_t ch);
	void CLKMODESet (ch_t ch);
	void CLOSEPLSMCUClr (ch_t ch);
	void CLOSEPLSMCUSet (ch_t ch);
	void FREQCTRLClr (ch_t ch);
	void FREQCTRLSet (ch_t ch);
	void GPIO_UserInit();
	FlagStatus GetNpn(ch_t ch);
	FlagStatus GetSTS(ch_t ch);
	FlagStatus GetPnp(ch_t ch);
	FlagStatus PLSSO (ch_t ch);
	/* ------------------------------------------------ */

#ifdef cplusplus
}
#endif

#endif
