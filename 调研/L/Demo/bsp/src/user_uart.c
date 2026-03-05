#include "user_uart.h"
#include <string.h>
#include "systick.h"

#define USART1_TX_BUF_SIZE		9
#define USART1_RX_BUF_SIZE		1029

/* IO array ------------------------------------------------------- */

#if APP
uint8_t* g_inaUartBuf;
#endif
/* ---------------------------------------------------------------- */
//#pragma arm section code = "RAMCODE"
uint8_t g_uart1Rxbuffer0[USART1_RX_BUF_SIZE];
uint8_t g_uart1Rxbuffer1[USART1_RX_BUF_SIZE];
uint8_t *g_uart1Rxbuffer[2] = {g_uart1Rxbuffer0, g_uart1Rxbuffer1};
uint8_t g_uart1Txbuf[] = "Run APP\r\n";
//#pragma arm section

static void UART_RccCfg (void)
{
    /* DMA clock enable */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_DMA, ENABLE);
    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(USER_COM_GPIO_CLK, ENABLE);
    /* Enable USARTx Clock */
    USER_COM_APBxClkCmd(USER_COM_CLK, ENABLE);
}

static void UART_GpioCfg (void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure USER_COM Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = USER_COM_TxPin;
	GPIO_InitStructure.GPIO_Pull      = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Mode      = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = USER_COM_Tx_GPIO_AF;
    GPIO_InitPeripheral(USER_COM_GPIO, &GPIO_InitStructure);
    
    /* Configure USER_COM Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = USER_COM_RxPin;
	GPIO_InitStructure.GPIO_Pull      = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = USER_COM_Rx_GPIO_AF;
	GPIO_InitPeripheral(USER_COM_GPIO, &GPIO_InitStructure);
}

static void UART_UserDMAInit (void)
{
    DMA_InitType DMA_InitStructure;

    /* USER_COM TX DMA1 Channel (triggered by USER_COM Tx event) Config */
    DMA_DeInit(USER_COM_Tx_DMA_Channel);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr     = USER_COM_DAT_Base;
    DMA_InitStructure.MemAddr        = (uint32_t)g_uart1Txbuf;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.BufSize        = USART1_TX_BUF_SIZE;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_BYTE;
    DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
    DMA_InitStructure.Priority       = DMA_PRIORITY_VERY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(USER_COM_Tx_DMA_Channel, &DMA_InitStructure);
    DMA_RequestRemap(USER_COM_Tx_DMA_REMAP, DMA, USER_COM_Tx_DMA_Channel, ENABLE);

    /* USER_COM RX DMA1 Channel (triggered by USARTy Rx event) Config */
    DMA_DeInit(USER_COM_Rx_DMA_Channel);
	DMA_InitStructure.PeriphAddr	 = USER_COM_DAT_Base;
    DMA_InitStructure.MemAddr		 = (uint32_t)g_uart1Rxbuffer0;
    DMA_InitStructure.Direction		 = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize		 = USART1_RX_BUF_SIZE;
    DMA_Init(USER_COM_Rx_DMA_Channel, &DMA_InitStructure);
    DMA_RequestRemap(USER_COM_Rx_DMA_REMAP, DMA, USER_COM_Rx_DMA_Channel, ENABLE);
}

void UART_UserInit (uint32_t baud)
{
	USART_InitType USART_InitStructure;
	
	/* System Clocks Configuration */
    UART_RccCfg();

    /* Configure the GPIO ports */
    UART_GpioCfg();

    /* Configure the DMA */
    UART_UserDMAInit();
	
	/* USARTy and USARTz configuration -------------------------------------------------------*/
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    /* Configure USARTy and USARTz */
    USART_Init(USER_COM, &USART_InitStructure);

    /* Enable USER_COM DMA Rx and TX request */
    USART_EnableDMA(USER_COM, USART_DMAREQ_RX | USART_DMAREQ_TX, ENABLE);

    /* Enable USARTy TX DMA Channel */
    DMA_EnableChannel(USER_COM_Tx_DMA_Channel, ENABLE);
    /* Enable USARTy RX DMA Channel */
    DMA_EnableChannel(USER_COM_Rx_DMA_Channel, ENABLE);

    /* Enable the USARTy and USARTz */
    USART_Enable(USER_COM, ENABLE);

#if APP
	/* Enable USARTy Receive and Transmit interrupts */
    USART_ConfigInt(USER_COM, USART_INT_IDLEF, ENABLE);
#endif
}

//#pragma arm section code = "RAMCODE"
#if BOOT
int32_t UART_RecvByte(uint8_t *buf, int32_t timeout)
{
	return UART_RecvData(buf, 1, timeout);
}

int32_t UART_RecvData (uint8_t *buf, int32_t size, int32_t timeout)
{
	uint32_t num = (size == -1) ? USART1_RX_BUF_SIZE : size;

	DMA_EnableChannel(USER_COM_Rx_DMA_Channel, DISABLE);
	DMA_ClearFlag(DMA_FLAG_TC5, DMA);
	DMA_SetCurrDataCounter(USER_COM_Rx_DMA_Channel, num);
	DMA_EnableChannel(USER_COM_Rx_DMA_Channel, ENABLE);

	while (RESET == USART_GetFlagStatus(USER_COM, USART_FLAG_IDLEF)) {
		if (timeout != -1) {
			if (timeout-- == 0) {
				return -1;
			}
			delay_1ms(1);
		}
	}

	/* clear IDLE flag */
	USART_ReceiveData(USER_COM);
	uint16_t rx_count = num - DMA_GetCurrDataCounter(USER_COM_Rx_DMA_Channel);
	if ((size != -1) && DMA_GetCurrDataCounter(USER_COM_Rx_DMA_Channel) != 0) {
		return -1;
	}
	memcpy(buf, (void*)g_uart1Rxbuffer0, rx_count);

	return 0;
}

int32_t UART_SendChar (uint8_t val, int32_t timeout)
{
	USART_SendData(USER_COM, val);
	while (RESET == USART_GetFlagStatus(USER_COM, USART_FLAG_TXDE)) {
		if (timeout != -1) {
			if (timeout-- == 0) {
				return -1;
			}
			delay_1ms(1);
		}
	}
	return 0;
}
#else
uint16_t UART_ToggleDmaBuf (void)
{
	static int32_t s_bufToggle = 0;

	s_bufToggle = 1 - s_bufToggle;
	uint32_t actUartBuf = (uint32_t)(g_uart1Rxbuffer[s_bufToggle]);
	g_inaUartBuf = g_uart1Rxbuffer[1 - s_bufToggle];

	uint16_t len = USART1_RX_BUF_SIZE - DMA_GetCurrDataCounter(USER_COM_Rx_DMA_Channel);
	DMA_EnableChannel(USER_COM_Rx_DMA_Channel, DISABLE);
	DMA_ClearFlag(DMA_FLAG_TC5, DMA);
	DMA_SetCurrDataCounter(USER_COM_Rx_DMA_Channel, USART1_RX_BUF_SIZE); // kFrameCnt
	USER_COM_Rx_DMA_Channel->MADDR = actUartBuf;
	DMA_EnableChannel(USER_COM_Rx_DMA_Channel, ENABLE);

	return len;
}

void UART_SendData (uint8_t* buf, uint32_t size)
{
	for (int i = 0; i < size; i++) {
		USART_SendData(USER_COM, buf[i]);
		while (RESET == USART_GetFlagStatus(USER_COM, USART_FLAG_TXDE)) {
		}
	}
}

void UART_DMA_SendData (uint8_t* buf, uint32_t size)
{
//	while(RESET == dma_flag_get(DMA0, DMA_CH6, DMA_FLAG_FTF));
	DMA_EnableChannel(USER_COM_Tx_DMA_Channel, DISABLE);
	DMA_ClearFlag(DMA_FLAG_TC4, DMA);
	USER_COM_Tx_DMA_Channel->MADDR = (uint32_t)buf;
	DMA_SetCurrDataCounter(USER_COM_Tx_DMA_Channel, size);
	DMA_EnableChannel(USER_COM_Tx_DMA_Channel, ENABLE);
	while(RESET == DMA_GetFlagStatus(DMA_FLAG_TC4, DMA));
}
#endif
//#pragma arm section
