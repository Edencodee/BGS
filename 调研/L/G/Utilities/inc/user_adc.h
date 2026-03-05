#ifndef __USER_ADC_H
#define __USER_ADC_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32c2x1.h"


#define SHOUT_PIN     GPIO_PIN_1
#define SHOUT_PORT 		GPIOA
#define SHOUT_CLK			RCU_GPIOA
#define SHOUT_CHANNEL			ADC_CHANNEL_1

	void ADC_UserInit (void);

#ifdef cplusplus
}
#endif

#endif
