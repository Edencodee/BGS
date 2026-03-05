#ifndef __UART_H
#define __UART_H

#ifdef cplusplus
extern "C" {
#endif

#include "n32l40x.h"

#define USER_COM                USART1
#define USER_COM_GPIO           GPIOA
#define USER_COM_CLK            RCC_APB2_PERIPH_USART1
#define USER_COM_GPIO_CLK       RCC_APB2_PERIPH_GPIOA
#define USER_COM_RxPin          GPIO_PIN_10
#define USER_COM_TxPin          GPIO_PIN_9
#define USER_COM_Rx_GPIO_AF     GPIO_AF4_USART1
#define USER_COM_Tx_GPIO_AF     GPIO_AF4_USART1
#define USER_COM_APBxClkCmd     RCC_EnableAPB2PeriphClk
#define USER_COM_DAT_Base       (USART1_BASE + 0x04)
#define USER_COM_Tx_DMA_Channel DMA_CH4
#define USER_COM_Tx_DMA_FLAG    DMA_FLAG_TC4
#define USER_COM_Rx_DMA_Channel DMA_CH5
#define USER_COM_Rx_DMA_FLAG    DMA_FLAG_TC5
#define USER_COM_Tx_DMA_REMAP   DMA_REMAP_USART1_TX
#define USER_COM_Rx_DMA_REMAP   DMA_REMAP_USART1_RX

#if APP
	extern uint8_t* g_inaUartBuf;

	uint16_t UART_ToggleDmaBuf (void);
	void UART_SendData(uint8_t* buf, uint32_t size);
	void UART_DMA_SendData(uint8_t* buf, uint32_t size);
#else
	int32_t UART_RecvData(uint8_t *buf, int32_t len, int32_t timeout);
	int32_t UART_RecvByte(uint8_t *buf, int32_t timeout);
	int32_t UART_SendChar(uint8_t val, int32_t timeout);
#endif

	void UART_UserInit(uint32_t baud);
//	void UART_UserDMAInit(void);

#ifdef cplusplus
}
#endif

#endif
