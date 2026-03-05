#include "main.h"

/* protocol function and variable ----------------------------------------------------------- */
static void DebugWEnHanler (uint16_t val);
static void CmdHanler      (uint16_t val);
static void WriteHanler    (uint16_t val);
static void ReadHanler     (uint16_t val);

typedef enum {
	kCmdDebugWEn = 0,
	kCmdCnt
} cmd_t;

const static uint16_t cmd[] = {
	[kCmdDebugWEn] = 0xA103,
};

const static handler_t cmdHandlers[] = {
	[kCmdDebugWEn] = DebugWEnHanler,
};

typedef enum {
	kRecvCmd = 0,
	kRecvWrite,
	kRecvRead,
	kRecvTypCnt
} recv_type_t;

const static handler_t recvHandlers[] = {
	[kRecvCmd    ] = CmdHanler,
	[kRecvWrite  ] = WriteHanler,
	[kRecvRead   ] = ReadHanler
};

const static uint8_t recvType[] = {kAtsCmd, kAtsWrite, kAtsRead};

/* 9. others variable ----------------------------------------------------------------------- */
//__IO int32_t g_adjCmdRecvCnt = 0;
__IO uint16_t g_lastReadReg = 0;

/* flag relative func ----------------------------------------------------------------------- */
static void DebugWEnHanler (uint16_t val)
{
//	static bool s_debugW = false;

//	if (s_debugW) {
//		ClrFlag(kFlagDebugW);
//		s_debugW = false;
//	} else {
	SetFlag(kFlagDebugW);
//		s_debugW = true;
//	}

	// Response(kAtsAck, val);
}
/* ------------------------------------------------------------------------------------------ */

/* response func ---------------------------------------------------------------------------- */
void Response (ats_byte_t cmd, uint16_t rsp)
{
	uint8_t buf[kFrameCnt] = {0};
	u16_t temp = {.word = rsp};

	buf[kFrameHead] = kAtsStx;
	buf[kFrameTpy ] = cmd;
	buf[kFrameAdd ] = g_sreg.addr;
	buf[kFrameMsb ] = temp.buf[kMsb];
	buf[kFrameLsb ] = temp.buf[kLsb];
	buf[kFrameBcc ] = buf[kFrameTpy] ^ g_sreg.addr ^ buf[kFrameMsb] ^ buf[kFrameLsb];

	UART_DMA_SendData(buf, kFrameCnt);
}
/* ------------------------------------------------------------------------------------------ */

/* response measure result relative func ---------------------------------------------------- */
/* ------------------------------------------------------------------------------------------ */

/* other handlers --------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------ */

static void CmdHanler (uint16_t val)
{
	for (int i = kCmdDebugWEn; i < kCmdCnt; i++) {
		if (cmd[i] == val) {
			cmdHandlers[i](val);
//			(i == kCmdReadMeas) ? SetFlag(kFlagLastRead) : ClrFlag(kFlagLastRead);
			return;
		}
	}
	Response(kAtsNak, kErrCmd);
}

static void WriteHanler (uint16_t val)
{
}

static void ReadHanler (uint16_t val)
{
}

bool AtspChk(uint8_t *p)
{
	return (p[kFrameHead] == kAtsStx) && \
	       (p[kFrameBcc] == (p[kFrameTpy] ^ g_sreg.addr ^ p[kFrameMsb] ^ p[kFrameLsb]));
}

void Atsp(uint8_t *p)
{
	for (int i = kRecvCmd; i < kRecvTypCnt; i++) {
		if (p[kFrameTpy] == recvType[i]) {	/* cmd / write / read */
			u16_t recv;
			recv.buf[kMsb] = p[kFrameMsb];
			recv.buf[kLsb] = p[kFrameLsb];
			/* CmdHandler() / WriteHandler() / ReadHandler() */
			recvHandlers[i](recv.word);
			return;
		}
	}
	Response(kAtsNak, kErrCmd);
}
