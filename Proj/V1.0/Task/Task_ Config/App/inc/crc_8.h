#ifndef __CRC_8_H__
#define __CRC_8_H__

#include <stdint.h>

uint8_t crc8_calc(const uint8_t *buf, uint8_t len);		//CRC-8校验

#endif
