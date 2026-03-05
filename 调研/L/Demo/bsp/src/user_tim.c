#include "user_tim.h"

static void TIM_NvicConfig(void)
{
    NVIC_InitType NVIC_InitStructure;

    /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

static void TIM_RccConfig(void) {
	/* PCLK1 = HCLK/4 */
    RCC_ConfigPclk1(RCC_HCLK_DIV4);

    /* TIM2 clock enable */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM2, ENABLE);
}

static void TIM_Config(void) {
	TIM_TimeBaseInitType TIM_TimeBaseStruct;

	/* Time base configuration */
	/* HSE=8M, PLL=64M, AHB=64M, APB1=16M, APB2=32M, TIM2_CLK=32M */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStruct);   
	TIM_TimeBaseStruct.Period    = 200;
	TIM_TimeBaseStruct.Prescaler = 31; // 1M = 32M / (31 + 1)
	TIM_TimeBaseStruct.ClkDiv    = 0;
	TIM_TimeBaseStruct.CntMode   = TIM_CNT_MODE_UP;
	TIM_InitTimeBase(TIM2, &TIM_TimeBaseStruct);
	
	/* Prescaler configuration */
    TIM_ConfigPrescaler(TIM2, 47, TIM_PSC_RELOAD_MODE_IMMEDIATE);

	TIM_ConfigInt(TIM2, TIM_INT_UPDATE, ENABLE);
	TIM_Enable(TIM2, ENABLE);
}

void TIM_UserInit(void)
{
	TIM_RccConfig();
	TIM_NvicConfig();
	TIM_Config();
}
