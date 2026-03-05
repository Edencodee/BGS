#include "user_gpio.h"

typedef enum {
	kBsIdle = 0,
	kBsDithering1,
	kBsWait1,
	kBsRelease1,
	kBsDithering2,
	kBsWait2,
	kBsWait3,
} button_state_t;

/* key variable */
__IO key_press_state_t g_pressState[PB_N] = {kNoneClick};

/* IO array -------------------------------------------------------------------------------- */
static GPIO_Module* PB_PORT[PB_N] = {PB1_PORT};
static uint16_t PB_PIN[PB_N] = {PB1_PIN};
static uint32_t PB_CLK[PB_N] = {PB1_CLK};

static GPIO_Module* NPN_PORT[OUT_N] = {NPN1_PORT};
static uint16_t NPN_PIN[OUT_N] = {NPN1_PIN};
static uint32_t NPN_CLK[OUT_N] = {NPN1_CLK};

static GPIO_Module* FUN_PORT[OUT_N] = {FUN1_PORT};
static uint16_t FUN_PIN[OUT_N] = {FUN1_PIN};
static uint32_t FUN_CLK[OUT_N] = {FUN1_CLK};

static GPIO_Module* PNP_PORT[OUT_N] = {PNP1_PORT};
static uint16_t PNP_PIN[OUT_N] = {PNP1_PIN};
static uint32_t PNP_CLK[OUT_N] = {PNP1_CLK};

static GPIO_Module* FUP_PORT[OUT_N] = {FUP1_PORT};
static uint16_t FUP_PIN[OUT_N] = {FUP1_PIN};
static uint32_t FUP_CLK[OUT_N] = {FUP1_CLK};

static GPIO_Module* LED_PORT[LED_N] = {LEDR_PORT, LEDG_PORT, LEDY_PORT};
static uint16_t LED_PIN[LED_N] = {LEDR_PIN, LEDG_PIN, LEDY_PIN};
static uint32_t LED_CLK[LED_N] = {LEDR_CLK, LEDG_CLK, LEDY_CLK};
/* ----------------------------------------------------------------------------------------- */
//#pragma arm section code = "RAMCODE"
/* output set/clr function ----------------------------------------------------------------- */
inline void NpnClr (ch_t ch)
{
	GPIO_ResetBits(NPN_PORT[ch], NPN_PIN[ch]);
}

inline void NpnSet (ch_t ch)
{
	GPIO_SetBits(NPN_PORT[ch], NPN_PIN[ch]);
}

inline void PnpClr(ch_t ch)
{
	GPIO_ResetBits(PNP_PORT[ch], PNP_PIN[ch]);
}

inline void PnpSet(ch_t ch)
{
	GPIO_SetBits(PNP_PORT[ch], PNP_PIN[ch]);
}
/* ----------------------------------------------------------------------------------------- */

/* protect get function -------------------------------------------------------------------- */
inline uint8_t GetNpn (ch_t ch)
{
	return GPIO_ReadInputDataBit(FUN_PORT[ch], FUN_PIN[ch]);
}

inline uint8_t GetPnp (ch_t ch)
{
	return GPIO_ReadInputDataBit(FUP_PORT[ch], FUP_PIN[ch]);
}
/* ----------------------------------------------------------------------------------------- */

/* switch get function --------------------------------------------------------------------- */
inline uint8_t GetPb (pb_t pb)
{
	return GPIO_ReadInputDataBit(PB_PORT[pb], PB_PIN[pb]);
}
/* ----------------------------------------------------------------------------------------- */

/* GPIO1 get function ---------------------------------------------------------------------- */
inline uint8_t GetGPIO1(void)
{
	return GPIO_ReadInputDataBit(GPIO1_PORT, GPIO1_PIN);
}
/* ----------------------------------------------------------------------------------------- */

/* XSHUT set/clr function ------------------------------------------------------------------ */
inline void XshutClr(void)
{
	GPIO_ResetBits(XSHUT_PORT, XSHUT_PIN);
}

inline void XshutSet(void)
{
	GPIO_SetBits(XSHUT_PORT, XSHUT_PIN);
}
/* ----------------------------------------------------------------------------------------- */

/* led set/clr function -------------------------------------------------------------------- */
inline void LedClr(led_t led)
{
	GPIO_ResetBits(LED_PORT[led], LED_PIN[led]);
}

inline void LedSet(led_t led)
{
	GPIO_SetBits(LED_PORT[led], LED_PIN[led]);
}
/* ----------------------------------------------------------------------------------------- */

/* SW set/clr function --------------------------------------------------------------------- */
inline void SwClr(void)
{
	GPIO_ResetBits(SW_PORT, SW_PIN);
}

inline void SwSet(void)
{
	GPIO_SetBits(SW_PORT, SW_PIN);
}
/* ----------------------------------------------------------------------------------------- */
//#pragma arm section
void GPIO_UserInit (void)
{
	GPIO_InitType initStruct;
	GPIO_InitStruct(&initStruct);
	
	/* 1. input ---------------------------------------------------------------------------- */
	/* configure PB GPIO port */
	initStruct.GPIO_Pull = GPIO_Pull_Up;
	initStruct.GPIO_Mode = GPIO_Mode_Input;
	for (int i = 0; i < PB_N; i++) {
		RCC_EnableAPB2PeriphClk(PB_CLK[i], ENABLE);
		initStruct.Pin = PB_PIN[i];
		GPIO_InitPeripheral(PB_PORT[i], &initStruct);		
	}
	
	/* configure FUN GPIO port*/
	for (int i = 0; i < OUT_N; i++) {
		RCC_EnableAPB2PeriphClk(FUN_CLK[i], ENABLE);
		initStruct.Pin = FUN_PIN[i];
		GPIO_InitPeripheral(FUN_PORT[i], &initStruct);	
	}
	
	/* configure FUP GPIO port*/
	for (int i = 0; i < OUT_N; i++) {
		RCC_EnableAPB2PeriphClk(FUP_CLK[i], ENABLE);
		initStruct.Pin = FUP_PIN[i];
		GPIO_InitPeripheral(FUP_PORT[i], &initStruct);	
	}
	
	/* GPIO1 configure as pull Down input*/
	initStruct.Pin = GPIO1_PIN;
	initStruct.GPIO_Pull = GPIO_Pull_Down;
	GPIO_InitPeripheral(GPIO1_PORT, &initStruct);

	/* 2. output --------------------------------------------------------------------------- */
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	/* configure NPN GPIO port*/
	initStruct.GPIO_Pull = GPIO_Pull_Up;
	for (int i = 0; i < OUT_N; i++) {
		RCC_EnableAPB2PeriphClk(NPN_CLK[i], ENABLE);
		initStruct.Pin = NPN_PIN[i];
		GPIO_InitPeripheral(NPN_PORT[i], &initStruct);
		NpnClr(kCh1);
	}

	/* configure PNP GPIO port*/
	initStruct.GPIO_Pull = GPIO_Pull_Down;
	for (int i = 0; i < OUT_N; i++) {
		RCC_EnableAPB2PeriphClk(PNP_CLK[i], ENABLE);
		initStruct.Pin = PNP_PIN[i];
		GPIO_InitPeripheral(PNP_PORT[i], &initStruct);
		PnpClr(kCh1);
	}
	
	/* configure LED GPIO port*/
	for (int i = 0; i < LED_N; i++) {
		RCC_EnableAPB2PeriphClk(LED_CLK[i], ENABLE);
		initStruct.Pin = LED_PIN[i];
		GPIO_InitPeripheral(LED_PORT[i], &initStruct);
		NpnClr(kCh1);
	}
	
	/* XSHUT configure as GPIO ouput push-pull */
	initStruct.Pin = XSHUT_PIN;
	GPIO_InitPeripheral(XSHUT_PORT, &initStruct);
	XshutClr();
	
	/* SW configure as GPIO ouput push-pull */
	initStruct.Pin = SW_PIN;
	GPIO_InitPeripheral(SW_PORT, &initStruct);
	
#if DEBUG
	SwSet();
#else	
	/* PA9 configure as GPIO output push-pull */
	initStruct.Pin = GPIO_PIN_9;
	GPIO_InitPeripheral(GPIOA, &initStruct);
	GPIOA->PBC = GPIO_PIN_9;
	
	SwClr();
#endif	
}

/* return 1 when key pressed */
static inline bool GPIO_isKeyActive (uint8_t index)
{
	return (0 == GetPb((pb_t)index));
}

/* call every 1ms */
void GPIO_PbScan (uint8_t index)
{
	static button_state_t s_keyState[PB_N + 1] = {kBsIdle};
	static uint16_t s_key_ticks[PB_N + 1] = {0};

	switch (s_keyState[index]) {
	case kBsIdle: {
		if (GPIO_isKeyActive(index)) {
			s_keyState[index] = kBsDithering1; /* press down switch to case 1 */
		}
	}
	break;

	case kBsDithering1: {
		if (GPIO_isKeyActive(index)) {
			/* dithering elimination */
			s_keyState[index] = kBsWait1;
			s_key_ticks[index] = 0;
		} else {
			s_keyState[index] = kBsIdle;
		}
	}
	break;

	case kBsWait1: {
		if (!GPIO_isKeyActive(index)) {
			/* release switch to case 3 */
			s_keyState[index] = kBsRelease1;
			s_key_ticks[index] = 0;
		} else if (s_key_ticks[index] > BS_LONG_TICKS) {
			/* long press switch to case 6 */
			g_pressState[index] = kLongClick;
			s_keyState[index] = kBsWait3;
		} else {
			s_key_ticks[index]++;
		}
	}
	break;

	case kBsRelease1: {
		if (GPIO_isKeyActive(index)) { /* press down again switch to case 4 */
			s_keyState[index] = kBsDithering2;
		} else if (s_key_ticks[index] > BS_SHORT_TMO) {
			/* shot press timeout indicating single pressed */
			g_pressState[index] = kSingleClick;
			s_keyState[index] = kBsIdle;
		} else {
			s_key_ticks[index]++;
		}
	}
	break;

	case kBsDithering2: {
		if (GPIO_isKeyActive(index)) { /* dithering elimination */
			/* set double pressed */
			g_pressState[index] = kDoubleClick;
			s_keyState[index] = kBsWait2;
		} else {
			s_keyState[index] = kBsRelease1;
		}
	}
	break;

	case kBsWait2: {
		if (!GPIO_isKeyActive(index)) {
			s_keyState[index] = kBsIdle;
		}
	}
	break;

	case kBsWait3: {
		if (!GPIO_isKeyActive(index)) {
			/* wait for release then set long pressed */
			g_pressState[index] = kLongClickRelease;
			s_keyState[index] = kBsIdle;
		}
	}
	break;
	}
}

key_press_state_t GPIO_ReadPbStatus (uint8_t index)
{
	key_press_state_t res = g_pressState[index];
	g_pressState[index] = kNoneClick;
	return res;
}
