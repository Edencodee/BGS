#ifndef __USER_SPI_H
#define __USER_SPI_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32c2x1.h"
#include "wtdint.h"

#define CAL_CLK_PIN     GPIO_PIN_3
#define CAL_CLK_PORT 		GPIOB
#define CAL_CLK_AF 			GPIO_AF_0
#define CAL_CLK_CLK			RCU_GPIOB
#define CAL_IN_PIN     	GPIO_PIN_5
#define CAL_IN_PORT 		GPIOB
#define CAL_IN_AF 			GPIO_AF_0
#define CAL_IN_CLK			RCU_GPIOB
#define TX_SPI					SPI0
#define TX_SPI_CLK			RCU_SPI0

#define TX_DMA					DMA_CH0

void SPI_UserInit (void);
void SPI_WritePD (void);

#ifdef cplusplus
}
#endif

#endif
