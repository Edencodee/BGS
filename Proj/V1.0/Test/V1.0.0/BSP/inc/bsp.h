#ifndef __BSP_H
#define __BSP_H


#ifdef __cplusplus
extern "C" {
#endif

#include "gd32c2x1.h"

/* include hardware */ 
#include "main.h"

void BSP_Init(void);
void ICPinConfig(void);

#ifdef __cplusplus
}
#endif

#endif
