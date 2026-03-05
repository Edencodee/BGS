#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H


#ifdef __cplusplus
extern "C" {
#endif

#include "gd32c2x1.h"

/* LED IO --------------------------------------------- */
#define LED_GREEN_PIN           GPIO_PIN_0
#define LED_GREEN_PORT          GPIOA
#define LED_GREEN_CLK           RCU_GPIOA

#define LED_YELLOW_PIN          GPIO_PIN_4
#define LED_YELLOW_PORT         GPIOA
#define LED_YELLOW_CLK          RCU_GPIOA

#define LED_N                   2U

/* NPN IO --------------------------------------------- */
#define NPN_PIN                 GPIO_PIN_14
#define NPN_PORT                GPIOC
#define NPN_CLK                 RCU_GPIOC
/* ---------------------------------------------------- */

/* FUN Input GPIO ------------------------------------- */
#define FUN_PIN                 GPIO_PIN_8
#define FUN_PORT                GPIOB
#define FUN_CLK                 RCU_GPIOB
/* ---------------------------------------------------- */

/* BTN IO --------------------------------------------- */
#define BTN_PIN                  GPIO_PIN_7
#define BTN_PORT                 GPIOA
#define BTN_CLK                  RCU_GPIOA
/* ---------------------------------------------------- */

/* BGS IC ============================================= */
/* CAL IO --------------------------------------------- */
#define CAL_IN_PIN           	GPIO_PIN_6
#define CAL_IN_PORT     		GPIOB
#define CAL_IN_CLK      		RCU_GPIOB

#define CAL_CLK_PIN           	GPIO_PIN_3
#define CAL_CLK_PORT     		GPIOB
#define CAL_CLK_CLK      		RCU_GPIOB

#define STS_PIN           		GPIO_PIN_0
#define STS_PORT     			GPIOB
#define STS_CLK      			RCU_GPIOB

#define CRC_RST_PIN           	GPIO_PIN_3
#define CRC_RST_PORT     		GPIOA
#define CRC_RST_CLK      		RCU_GPIOA
/* ---------------------------------------------------- */



/* CLK_MODE IO ----------------------------------------- */
#define CLK_MODE_PIN             GPIO_PIN_5
#define CLK_MODE_PORT            GPIOA
#define CLK_MODE_CLK             RCU_GPIOA
/* ---------------------------------------------------- */

/* FREQ_CTRL IO ----------------------------------------- */
#define FREQ_CTRL_PIN             GPIO_PIN_15
#define FREQ_CTRL_PORT            GPIOA
#define FREQ_CTRL_CLK             RCU_GPIOA
/* ---------------------------------------------------- */

/* CLOSE_PLS IO ----------------------------------------- */
#define CLOSE_PLS_PIN             GPIO_PIN_5
#define CLOSE_PLS_PORT            GPIOB
#define CLOSE_PLS_CLK             RCU_GPIOB
/* ---------------------------------------------------- */

/* CLK Output IO -------------------------------------- */
#define EXT_CLK_PORT			  	GPIOA
#define EXT_CLK_CLK      		  	RCU_GPIOA

//EXT_LS_CLK
#define EXT_LS_CLK_PIN 				GPIO_PIN_10
//EXT_HS_CLK
#define EXT_HS_CLK_PIN 				GPIO_PIN_9
/* ---------------------------------------------------- */

/* LED typedef ---------------------------------------- */
typedef enum {
	kLedGreen = 0,
	kLedYellow
} led_t;
/* ---------------------------------------------------- */

/* function declaration ------------------------------- */
void LED_Set(led_t led);
void LED_Clr(led_t led);
void NPN_Set(void);
void NPN_Clr(void);
void CrcRstSet(void);
void CrcRstClr(void);
uint8_t STSGetState(void);
void CLKMode_Set(void);
void CLKMode_Clr(void);
void FreqCtrl_Set(void);
void FreqCtrl_Clr(void);
void ClosePLS_Set(void);
void ClosePLS_Clr(void);
FlagStatus FUN_GetState (void);
void GPIO_UserInit(void);
/* ---------------------------------------------------- */



#ifdef __cplusplus
}
#endif

#endif
