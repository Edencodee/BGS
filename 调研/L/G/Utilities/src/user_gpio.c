#include "user_gpio.h"
#include <string.h>

/* IO array -------------------------------------------------------------------------------- */
const uint32_t LED_PORT[LED_N] = {GLED_PORT, YLED_PORT};
const uint32_t LED_PIN[LED_N] = {GLED_PIN, YLED_PIN};
const rcu_periph_enum LED_CLK[LED_N] = {GLED_CLK, YLED_CLK};

const uint32_t NPN_PORT[OUT_N] = {NPN1_PORT};
const uint32_t NPN_PIN[OUT_N] = {NPN1_PIN};
const rcu_periph_enum NPN_CLK[OUT_N] = {NPN1_CLK};

const uint32_t PNP_PORT[OUT_N] = {PNP1_PORT};
const uint32_t PNP_PIN[OUT_N] = {PNP1_PIN};
const rcu_periph_enum PNP_CLK[OUT_N] = {PNP1_CLK};

const uint32_t FUN_PORT[OUT_N] = {FUN1_PORT};
const uint32_t FUN_PIN[OUT_N] = {FUN1_PIN};
const rcu_periph_enum FUN_CLK[OUT_N] = {FUN1_CLK};

const uint32_t FUP_PORT[OUT_N] = {FUP1_PORT};
const uint32_t FUP_PIN[OUT_N] = {FUP1_PIN};
const rcu_periph_enum FUP_CLK[OUT_N] = {FUP1_CLK};

const uint32_t LD_PORT[LD_N] = {LD1_PORT};
const uint32_t LD_PIN[LD_N] = {LD1_PIN};
const rcu_periph_enum LD_CLK[LD_N] = {LD1_CLK};

const uint32_t STS_OUT_PORT[PD_N] = {STS_OUT1_PORT};
const uint32_t STS_OUT_PIN[PD_N] = {STS_OUT1_PIN};
const rcu_periph_enum STS_OUT_CLK[PD_N] = {STS_OUT1_CLK};

const uint32_t CRC_RST_PORT[PD_N] = {CRC_RST1_PORT};
const uint32_t CRC_RST_PIN[PD_N] = {CRC_RST1_PIN};
const rcu_periph_enum CRC_RST_CLK[PD_N] = {CRC_RST1_CLK};

const uint32_t PLS_SO_PORT[PD_N] = {PLS_SO1_PORT};
const uint32_t PLS_SO_PIN[PD_N] = {PLS_SO1_PIN};
const rcu_periph_enum PLS_SO_CLK[PD_N] = {PLS_SO1_CLK};

const uint32_t CLK_MODE_PORT[PD_N] = {CLK_MODE1_PORT};
const uint32_t CLK_MODE_PIN[PD_N] = {CLK_MODE1_PIN};
const rcu_periph_enum CLK_MODE_CLK[PD_N] = {CLK_MODE1_CLK};

const uint32_t CLOSE_PLS_MCU_PORT[PD_N] = {CLOSE_PLS_MCU1_PORT};
const uint32_t CLOSE_PLS_MCU_PIN[PD_N] = {CLOSE_PLS_MCU1_PIN};
const rcu_periph_enum CLOSE_PLS_MCU_CLK[PD_N] = {CLOSE_PLS_MCU1_CLK};

const uint32_t FREQ_CTRL_PORT[PD_N] = {FREQ_CTRL1_PORT};
const uint32_t FREQ_CTRL_PIN[PD_N] = {FREQ_CTRL1_PIN};
const rcu_periph_enum FREQ_CTRL_CLK[PD_N] = {FREQ_CTRL1_CLK};
/* ----------------------------------------------------------------------------------------- */

/* output set/clr function ----------------------------------------------------------------- */
inline void NpnClr (ch_t ch)
{
	GPIO_BC(NPN_PORT[ch]) = NPN_PIN[ch];
}

inline void NpnSet (ch_t ch)
{
	GPIO_BOP(NPN_PORT[ch]) = NPN_PIN[ch];
}

inline void PnpClr (ch_t ch)
{
	GPIO_BC(PNP_PORT[ch]) = PNP_PIN[ch];
}

inline void PnpSet (ch_t ch)
{
	GPIO_BOP(PNP_PORT[ch]) = PNP_PIN[ch];
}
/* ----------------------------------------------------------------------------------------- */

/* led set/clr function -------------------------------------------------------------------- */
inline void LedClr (led_t led)
{
	GPIO_BC(LED_PORT[led]) = LED_PIN[led];
}

inline void LedSet (led_t led)
{
	GPIO_BOP(LED_PORT[led]) = LED_PIN[led];
}
/* ----------------------------------------------------------------------------------------- */

/* protect get function -------------------------------------------------------------------- */
inline FlagStatus GetNpn (ch_t ch)
{
	return gpio_input_bit_get(FUN_PORT[ch], FUN_PIN[ch]);
}

inline FlagStatus GetPnp (ch_t ch)
{
	return gpio_input_bit_get(FUP_PORT[ch], FUP_PIN[ch]);
}

inline FlagStatus GetSTS (ch_t ch)
{
	return gpio_input_bit_get(STS_OUT_PORT[ch], STS_OUT_PIN[ch]);
}
/* ----------------------------------------------------------------------------------------- */

/* ld set/clr function -------------------------------------------------------------------- */
inline void LDClr (ch_t ch)
{
	GPIO_BC(LD_PORT[ch]) = LD_PIN[ch];
}

inline void LDSet (ch_t ch)
{
	GPIO_BOP(LD_PORT[ch]) = LD_PIN[ch];
}
/* ----------------------------------------------------------------------------------------- */

/* PD set/clr function -------------------------------------------------------------------- */
inline void CRCRSTClr (ch_t ch)
{
	GPIO_BC(CRC_RST_PORT[ch]) = CRC_RST_PIN[ch];
}

inline void CRCRSTSet (ch_t ch)
{
	GPIO_BOP(CRC_RST_PORT[ch]) = CRC_RST_PIN[ch];
}

inline FlagStatus STSOUT (ch_t ch)
{
	return gpio_input_bit_get(FUN_PORT[ch], FUN_PIN[ch]);
}

inline FlagStatus PLSSO (ch_t ch)
{
	return gpio_input_bit_get(FUN_PORT[ch], FUN_PIN[ch]);
}

inline void CLKMODEClr (ch_t ch)
{
	GPIO_BC(CLK_MODE_PORT[ch]) = CLK_MODE_PIN[ch];
}

inline void CLKMODESet (ch_t ch)
{
	GPIO_BOP(CLK_MODE_PORT[ch]) = CLK_MODE_PIN[ch];
}

inline void CLOSEPLSMCUClr (ch_t ch)
{
	GPIO_BC(CLOSE_PLS_MCU_PORT[ch]) = CLOSE_PLS_MCU_PIN[ch];
}

inline void CLOSEPLSMCUSet (ch_t ch)
{
	GPIO_BOP(CLOSE_PLS_MCU_PORT[ch]) = CLOSE_PLS_MCU_PIN[ch];
}

inline void FREQCTRLClr (ch_t ch)
{
	GPIO_BC(FREQ_CTRL_PORT[ch]) = FREQ_CTRL_PIN[ch];
}

inline void FREQCTRLSet (ch_t ch)
{
	GPIO_BOP(FREQ_CTRL_PORT[ch]) = FREQ_CTRL_PIN[ch];
}
///* ----------------------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------------------- */
void GPIO_UserInit()
{
	/* configure LED GPIO port */
	for (int i = 0; i < LED_N; i++) {
		rcu_periph_clock_enable(LED_CLK[i]);
		gpio_mode_set(LED_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN[i]);
		gpio_output_options_set(LED_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, LED_PIN[i]);
		GPIO_BC(LED_PORT[i]) = LED_PIN[i];
	}

	/* configure NPN/PNP GPIO port*/
	for (int i = 0; i < OUT_N; i++) {
		rcu_periph_clock_enable(NPN_CLK[i]);
		gpio_mode_set(NPN_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, NPN_PIN[i]);
		gpio_output_options_set(NPN_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, NPN_PIN[i]);
		GPIO_BC(NPN_PORT[i]) = NPN_PIN[i];

		rcu_periph_clock_enable(PNP_CLK[i]);
		gpio_mode_set(PNP_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PNP_PIN[i]);
		gpio_output_options_set(PNP_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, PNP_PIN[i]);
		GPIO_BC(PNP_PORT[i]) = PNP_PIN[i];
	}

	/* configure FUN/FUP GPIO port*/
	for (int i = 0; i < OUT_N; i++) {
		rcu_periph_clock_enable(FUN_CLK[i]);
		gpio_mode_set(FUN_PORT[i], GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, FUN_PIN[i]);

		rcu_periph_clock_enable(FUP_CLK[i]);
		gpio_mode_set(FUP_PORT[i], GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, FUP_PIN[i]);
	}
	
		/* configure LD GPIO port*/
	for (int i = 0; i < LD_N; i++) {
		rcu_periph_clock_enable(LD_CLK[i]);
		gpio_mode_set(LD_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LD_PIN[i]);
		gpio_output_options_set(LD_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, LD_PIN[i]);
		GPIO_BC(LD_PORT[i]) = LD_PIN[i];
	}
	
					/* configure STS_OUT GPIO port*/
	for (int i = 0; i < PD_N; i++) {
		rcu_periph_clock_enable(STS_OUT_CLK[i]);
		gpio_mode_set(STS_OUT_PORT[i], GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, STS_OUT_PIN[i]);
	}
	
		/* configure CRC_RST GPIO port*/
	for (int i = 0; i < PD_N; i++) {
		rcu_periph_clock_enable(CRC_RST_CLK[i]);
		gpio_mode_set(CRC_RST_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CRC_RST_PIN[i]);
		gpio_output_options_set(CRC_RST_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, CRC_RST_PIN[i]);
		GPIO_BC(CRC_RST_PORT[i]) = CRC_RST_PIN[i];
	}
	
				/* configure PLS_SO GPIO port*/
	for (int i = 0; i < PD_N; i++) {
		rcu_periph_clock_enable(PLS_SO_CLK[i]);
		gpio_mode_set(PLS_SO_PORT[i], GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, PLS_SO_PIN[i]);
	}
	
			/* configure CLK_MODE GPIO port*/
	for (int i = 0; i < PD_N; i++) {
		rcu_periph_clock_enable(CLK_MODE_CLK[i]);
		gpio_mode_set(CLK_MODE_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CLK_MODE_PIN[i]);
		gpio_output_options_set(CLK_MODE_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, CLK_MODE_PIN[i]);
		GPIO_BC(CLK_MODE_PORT[i]) = CLK_MODE_PIN[i];
	}
	
					/* configure CLOSE_PLS_MCU GPIO port*/
	for (int i = 0; i < PD_N; i++) {
		rcu_periph_clock_enable(CLOSE_PLS_MCU_CLK[i]);
		gpio_mode_set(CLOSE_PLS_MCU_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CLOSE_PLS_MCU_PIN[i]);
		gpio_output_options_set(CLOSE_PLS_MCU_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, CLOSE_PLS_MCU_PIN[i]);
		GPIO_BC(CLOSE_PLS_MCU_PORT[i]) = CLOSE_PLS_MCU_PIN[i];
	}
	
						/* configure FREQ_CTRL GPIO port*/
	for (int i = 0; i < PD_N; i++) {
		rcu_periph_clock_enable(FREQ_CTRL_CLK[i]);
		gpio_mode_set(FREQ_CTRL_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, FREQ_CTRL_PIN[i]);
		gpio_output_options_set(FREQ_CTRL_PORT[i], GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, FREQ_CTRL_PIN[i]);
		GPIO_BC(FREQ_CTRL_PORT[i]) = LD_PIN[i];
	}
}
