#include "user_flash.h"
#include <math.h>
#include <string.h>
#include "wtdint.h"
#include "user_gpio.h"

#if APP
__IO s_reg_t __attribute__((section(".RAMDATA"))) g_sreg;
__IO uint16_t *g_rom = (uint16_t*)REG_ADDR;

s_reg_t sregDef = {
	.state   = kLonBgs,
	.p1orp2  = 0,
	.cali1p1 = 3000,
	.cali1p2 = 3000,
	.cali2p2 = 3000,
};

s_reg_t sregMax = {
	.state   = kDonBgs,
	.p1orp2  = 1,
	.cali1p1 = 6000,
	.cali1p2 = 6000,
	.cali2p2 = 6000,
};

s_reg_t sregMin = {
	.state   = kLonBgs,
	.p1orp2  = 0,
	.cali1p1 = 100,
	.cali1p2 = 100,
	.cali2p2 = 100,
};

__IO uint16_t *g_reg = (uint16_t*)&g_sreg;
uint16_t *regDef = (uint16_t*)&sregDef;
uint16_t *regMax = (uint16_t*)&sregMax;
uint16_t *regMin = (uint16_t*)&sregMin;
#endif

void FLASH_UserErase (uint32_t pageNum)
{
	/* unlock the flash program/erase controller */
	FLASH_Unlock();

	/* clear all pending flags */
	FLASH_ClearFlag(FLASH_STS_CLRFLAG);

	/* wait the erase operation complete*/
	FLASH_EraseOnePage(0x8000000ul + (PAGE_SIZE * pageNum));

	/* clear all pending flags */
	FLASH_ClearFlag(FLASH_STS_CLRFLAG);

	/* lock the main FMC after the erase operation */
	FLASH_Lock();
}

void FLASH_UserWrite (uint32_t addr, uint32_t *buf, uint16_t size)
{
	uint16_t i;

	/* unlock the flash program/erase controller */
	FLASH_Unlock();

	/* Erase */
	if (FLASH_COMPL != FLASH_EraseOnePage(addr)) {
		while(1) {
			printf("Flash EraseOnePage Error. Please Deal With This Error Promptly\r\n");
		}
	}

	/* clear all pending flags */
	FLASH_ClearFlag(FLASH_STS_CLRFLAG);

	/* program flash */
	for (i = 0; i < size; i++) {
		FLASH_ProgramWord(addr, buf[i]);
		addr += 4;
		/* clear all pending flags */
		FLASH_ClearFlag(FLASH_STS_CLRFLAG);
	}

	/* lock the main FMC after the program operation */
	FLASH_Lock();
}
#if APP
/* write function -------------------------------------------------------- */
void FLASH_RegWrite (void)
{
	__disable_irq();
	
	FLASH_UserErase(8); /* page 8 */
	FLASH_UserErase(9); /* page 9 */
	FLASH_UserWrite(REG_ADDR, (uint32_t*)g_reg, sizeof(s_reg_t) / 4);
	
	__enable_irq();
}

/* load&check function -------------------------------------------------- */
static int  FLASH_RegsChk (int start, int cnt)
{
	int modify = 0;

	for(int i = 0; i < kRegSetCnt; i++) {
		if((g_reg[i] < regMin[i]) || (g_reg[i] > regMax[i])) {
			g_reg[i] = regDef[i];
			modify = 1;
		}
	}

	return modify;
}

static void FLASH_LoadData (uint32_t addr, uint16_t size, uint32_t buf)
{
	uint32_t i;
	uint32_t *ptrd = (uint32_t*)addr;
	uint32_t *reg32 = (uint32_t*)buf;

	for (i = 0; i < size; i++) {
		reg32[i] = ptrd[i];
	}
}

void FLASH_RegLoadChk (void)
{
	int32_t modify = 0;

	FLASH_LoadData(REG_ADDR, sizeof(s_reg_t) / 4, (uint32_t)g_reg);
	g_sreg.distance = 0;
	g_sreg.rangeStatus = 0;
	g_sreg.btnLed[kBLed] = kLedNone;
	g_sreg.btnLed[kYLed] = kLedNone;
	g_sreg.btnLed[kRLed] = kLedNone;
	g_sreg.flag = 0;
	
	if (FLASH_RegsChk(0, kRegSetCnt)) {
		modify = 1;
	}

	if (modify) {
		SetFlag(kFlagWriteReg);
	}
}
/* ---------------------------------------------------------------------- */
/* reg init funtion ----------------------------------------------------- */
static void FLASH_RegsReset (int start, int cnt)
{
	for (int i = start; i < cnt; i++) {
		g_reg[i] = regDef[i];
	}
}

void FLASH_RegReinit (void)
{
	FLASH_RegsReset(0, kRegSetCnt);
	FLASH_RegWrite();
}
/* ---------------------------------------------------------------------- */

/* flag funtion --------------------------------------------------------- */
inline bool ChkFlag (flag_t flag)
{
	return (0 != (g_sreg.flag & (1 << flag)));
}

inline void SetFlag (flag_t flag)
{
	g_sreg.flag |= (1 << flag);
}

inline void ClrFlag (flag_t flag)
{
	g_sreg.flag &= ~(1 << flag);
}
/* ---------------------------------------------------------------------- */
#endif
