#include "bsp_gpio.h"

/* IO array for LED ------------------------------------------------------------------- */
const uint32_t LED_PORT[LED_N] = {LED_GREEN_PORT, LED_YELLOW_PORT};
const uint32_t LED_PIN[LED_N] = {LED_GREEN_PIN, LED_YELLOW_PIN};
const rcu_periph_enum LED_CLK[LED_N] = {LED_GREEN_CLK, LED_YELLOW_CLK};

/* -------------------------------------------------------------------------------------- */


/* LED functions ------------------------------------------------------------------- */
void LedSet (led_t led)
{
	GPIO_BOP(LED_PORT[led]) = LED_PIN[led];
}

void LedClr (led_t led)
{
	GPIO_BC(LED_PORT[led]) = LED_PIN[led];
}

/* NPN functions ------------------------------------------------------------------ */
void NPNSet (void)
{
	GPIO_BOP(NPN_PORT) = NPN_PIN;
}

void NPNClr (void)
{
	GPIO_BC(NPN_PORT) = NPN_PIN;
}

/* FUN Input function ------------------------------------------------------------ */
FlagStatus FUNGetState (void)
{
	return gpio_input_bit_get(FUN_PORT, FUN_PIN);
}

/* BTN Input function ------------------------------------------------------------ */
btn_state_t BtnGetState (void)
{
	return (btn_state_t)gpio_input_bit_get(BTN_PORT, BTN_PIN);
}


/* CLK_MODE Output function ------------------------------------------------------ */
void ClkModeSet (void)
{
	GPIO_BOP(CLK_MODE_PORT) = CLK_MODE_PIN;
}

void ClkModeClr (void)
{
	GPIO_BC(CLK_MODE_PORT) = CLK_MODE_PIN;
}

/* FREQ_CTRL Output function ----------------------------------------------------- */
void FreqCtrlSet (void)
{
	GPIO_BOP(FREQ_CTRL_PORT) = FREQ_CTRL_PIN;
}

void FreqCtrlClr (void)
{
	GPIO_BC(FREQ_CTRL_PORT) = FREQ_CTRL_PIN;
}

/* CLOSE_PLS Output function ----------------------------------------------------- */
void ClosePlsSet (void)
{
	GPIO_BOP(CLOSE_PLS_PORT) = CLOSE_PLS_PIN;
}

void ClosePlsClr (void)
{
	GPIO_BC(CLOSE_PLS_PORT) = CLOSE_PLS_PIN;
}
/* -------------------------------------------------------------------------------------- */

void GPIO_UserInit(void)
{
	/* configure LED GPIO port (output) */
	for (int i = 0; i < LED_N; i++) {
		rcu_periph_clock_enable(LED_CLK[i]);
		gpio_mode_set(LED_PORT[i], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN[i]);
		GPIO_BC(LED_PORT[i]) = LED_PIN[i];
	}
	/* configure NPN GPIO port (output) */
	rcu_periph_clock_enable(NPN_CLK);
	gpio_mode_set(NPN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, NPN_PIN);
	GPIO_BC(NPN_PORT) = NPN_PIN;

	/* configure FUN GPIO port (input) */
	rcu_periph_clock_enable(FUN_CLK);
	gpio_mode_set(FUN_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, FUN_PIN);
	
	/* configure BTN GPIO port (input) */
	rcu_periph_clock_enable(BTN_CLK);
	gpio_mode_set(BTN_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, BTN_PIN);


	/* configure CLK_MODE GPIO port (output, default low) */
	rcu_periph_clock_enable(CLK_MODE_CLK);
	gpio_mode_set(CLK_MODE_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CLK_MODE_PIN);
	GPIO_BC(CLK_MODE_PORT) = CLK_MODE_PIN;

	/* configure FREQ_CTRL GPIO port (output, default low) */
	rcu_periph_clock_enable(FREQ_CTRL_CLK);
	gpio_mode_set(FREQ_CTRL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, FREQ_CTRL_PIN);
	GPIO_BOP(FREQ_CTRL_PORT) = FREQ_CTRL_PIN;

	/* configure CLOSE_PLS GPIO port (output, default low) */
	rcu_periph_clock_enable(CLOSE_PLS_CLK);
	gpio_mode_set(CLOSE_PLS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CLOSE_PLS_PIN);
	GPIO_BOP(CLOSE_PLS_PORT) = CLOSE_PLS_PIN;

	/* IC Config & GPIO BitBang ================================================== */
	/* configure CAL GPIO port (output, default low) */
	rcu_periph_clock_enable(CAL_CLK_CLK);
	gpio_mode_set(CAL_CLK_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CAL_CLK_PIN);
	gpio_output_options_set(CAL_CLK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, CAL_CLK_PIN);
	GPIO_BC(CAL_CLK_PORT) = CAL_CLK_PIN;	

	/* configure CAL_IN GPIO port (output, default low) */
	rcu_periph_clock_enable(CAL_IN_CLK);
	gpio_mode_set(CAL_IN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CAL_IN_PIN);
	gpio_output_options_set(CAL_IN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, CAL_IN_PIN);
	GPIO_BC(CAL_IN_PORT) = CAL_IN_PIN;

	/* configure STS GPIO port (input) */
	rcu_periph_clock_enable(STS_CLK);
	gpio_mode_set(STS_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, STS_PIN);

	/* configure CRC_RST GPIO port (output, default low) */
	rcu_periph_clock_enable(CRC_RST_CLK);
	gpio_mode_set(CRC_RST_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CRC_RST_PIN);
	gpio_output_options_set(CRC_RST_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_LEVEL_1, CRC_RST_PIN);
	GPIO_BC(CRC_RST_PORT) = CRC_RST_PIN;

	/* ================= IC Config & GPIO BitBang ================================== */

	/* configure MCO GPIO port */
	rcu_periph_clock_enable(EXT_CLK_CLK);
	//HS -> PA9
	gpio_mode_set(EXT_CLK_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EXT_HS_CLK_PIN);
	gpio_af_set(EXT_CLK_PORT, GPIO_AF_0, EXT_HS_CLK_PIN);
	rcu_ckout0_config(RCU_CKOUT0SRC_CKSYS, RCU_CKOUT0_DIV2);
	//LS -> PA10
	gpio_mode_set(EXT_CLK_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EXT_LS_CLK_PIN);
	gpio_af_set(EXT_CLK_PORT, GPIO_AF_3, EXT_LS_CLK_PIN);
	rcu_ckout1_config(RCU_CKOUT1SRC_CKSYS,RCU_CKOUT1_DIV4);

}


