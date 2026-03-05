#include "main.h"

void FlashTask (void)
{
	/* 1. Reg write */
	if (ChkFlag(kFlagWriteReg)) {
		/* 1.1. ... */
		// todo...
		
		/* 1.x. write execute */
		FLASH_RegWrite();
		printf("FLASH_RegWrite");
		ClrFlag(kFlagWriteReg);
	}
}
