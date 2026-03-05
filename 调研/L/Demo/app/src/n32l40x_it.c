#include "main.h"

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 */
void DebugMon_Handler(void)
{
}

//#pragma arm section code = "RAMCODE"
/**
 * @brief  This function handles SysTick Handler.
 */
void SysTick_Handler(void)
{
	delay_decrement();
	SysTickCallBack();
}

/******************************************************************************/
/*                 N32L40X Peripherals Interrupt Handlers                     */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_n32l40x.s).                                                 */
/******************************************************************************/
/**
 * @brief  This function handles TIM2_IRQHandler exception.
 */
void TIM2_IRQHandler(void)
{
	if (TIM_GetIntStatus(TIM2, TIM_INT_UPDATE) != RESET) {
		TIM_ClrIntPendingBit(TIM2, TIM_INT_UPDATE);
		
		Timer2UpdateCallBack();
	}
}

/**
 * @brief  This function handles USART1_IRQHandler exception.
 */
void USART1_IRQHandler(void)
{
	if (USART_GetIntStatus(USER_COM, USART_INT_IDLEF) != RESET) {
		USART_ReceiveData(USER_COM);
		
		/* toggle buffer */
		uint16_t len = UART_ToggleDmaBuf();
		RecvIdleCallBack(g_inaUartBuf, len);
	}
}
//#pragma arm section

