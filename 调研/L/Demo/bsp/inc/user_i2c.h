#ifndef __User_I2C_H
#define __User_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "n32l40x.h"

void I2C_UserInit(void);
int I2C_MasterSend(uint8_t dev, uint8_t* p, int size);
int I2C_MasterRecv(uint8_t dev, uint8_t* p, int size);

#ifdef __cplusplus
}
#endif

#endif
