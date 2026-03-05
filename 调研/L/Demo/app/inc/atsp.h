#ifndef __ATSP_H
#define __ATSP_H

#ifdef cplusplus
extern "C" {
#endif

#include "main.h"

	typedef enum {
		kFrameHead = 0,
		kFrameTpy,
		kFrameAdd,
		kFrameMsb,
		kFrameLsb,
		kFrameBcc,
		kFrameCnt
	} frame_t;

	typedef enum {
		kAtsStx   = 0x02,
		kAtsCmd   = 0x43,
		kAtsWrite = 0x57,
		kAtsRead  = 0x52,
		kAtsAck   = 0x06,
		kAtsNak   = 0x15,
		kAtsEtx   = 0x03
	} ats_byte_t;

	typedef enum {
		kErrNone   = 0x0000,
		kErrAdjust = 0x0100,
		kErrAddr   = 0x0200,
		kErrBcc    = 0x0400,
		kErrCmd    = 0x0500,
		kErrPara   = 0x0600,
		kErrRange  = 0x0700,
		kErrSearch = 0x0800,
	} err_t;

	typedef void (*handler_t)(uint16_t val);

	bool AtspChk(uint8_t *p);
	void Atsp(uint8_t *p);
	void Response (ats_byte_t cmd, uint16_t rsp);

#ifdef cplusplus
}
#endif

#endif
