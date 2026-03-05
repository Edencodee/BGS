#include "main.h"

#define BPS_N 7

void FlashTask (void)
{
	/* 1. Reg write */
	if (ChkFlag(kFlagWriteReg)) {
		/* 1.1. ... */
		// todo...
		
		/* 1.x. write execute */
		FLASH_RegWrite();
		wlog("FLASH_RegWrite");
		ClrFlag(kFlagWriteReg);
	}
}
