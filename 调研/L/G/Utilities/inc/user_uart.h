#ifndef __USER_UART_H
#define __USER_UART_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32c2x1.h"
#include "wtdint.h"

#define CAL_IN_PIN     	GPIO_PIN_6
#define CAL_IN_PORT 		GPIOB
#define CAL_IN_AF 			GPIO_AF_0
#define CAL_IN_CLK			RCU_GPIOB

void com_usart_init(void);

#ifdef cplusplus
}
#endif

#endif
