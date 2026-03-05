#ifndef __TASK_H
#define __TASK_H

#ifdef cplusplus
extern "C" {
#endif

#include "main.h"

#if DOUBLE_ELE
	typedef double ele_t;
#else
	typedef float ele_t;
#endif

#define UART_SEND_DATA(buf, len) UART_DMA_SendData(USART1, buf, len)

	void AppInit(void);
	void RecvTimeoutCallBack(void* userData, uint16_t len);
	void HandleCSIFrame(uint16_t* p);
	void SysTickCallBack(void);
	void Timer3UpdateCallBack(void);

#ifdef cplusplus
}
#endif

#endif
