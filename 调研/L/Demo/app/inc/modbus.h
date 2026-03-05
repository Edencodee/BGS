#ifndef __MODBUS_H
#define __MODBUS_H

#ifdef cplusplus
extern "C" {
#endif

#include "main.h"

	void MODBUS_ContiuneOut (void);
	void MODBUS_RecvHandler(uint8_t* userData, uint16_t len);

#ifdef cplusplus
}
#endif

#endif
