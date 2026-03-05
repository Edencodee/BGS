#include "user_flash.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "wtdint.h"

__IO s_reg_t __attribute__((section(".RAMDATA"))) g_sreg;
__IO uint16_t *g_reg = (uint16_t*)&g_sreg;

s_reg_t sregDef = {
	.state   = kLonBgs,
	.cali1p1 = 3000,
	.cali1p2 = 3000,
	.cali2p2 = 3000,
};

s_reg_t sregMax = {
	.state   = kDonBgs,
	.cali1p1 = 6000,
	.cali1p2 = 6000,
	.cali2p2 = 6000,
};

s_reg_t sregMin = {
	.state   = kLonBgs,
	.cali1p1 = 100,
	.cali1p2 = 100,
	.cali2p2 = 100,
};
uint16_t *regDef = (uint16_t*)&sregDef;
uint16_t *regMax = (uint16_t*)&sregMax;
uint16_t *regMin = (uint16_t*)&sregMin;

#define REG_ADDR								((uint32_t)0x08002000U)	/* REG   4k, sector  8~ 9, 0x8004000~0x8004fff	*/


void FLASH_UserErase(uint32_t pageNum)
{
    /* unlock register FMC_CTL */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_ENDF | FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_OPRERR | FMC_FLAG_PGSERR | FMC_FLAG_PGMERR | FMC_FLAG_PGAERR);
   
    /* erase the flash pages */
		fmc_page_erase(pageNum);

    /* lock register FMC_CTL after the erase operation */
    fmc_lock();
}

void FLASH_UserWrite (uint32_t addr, uint32_t *buf, uint16_t size)
{
		uint16_t i;
	 /* unlock register FMC_CTL */
    fmc_unlock();

    /* program flash */
		for (i = 0; i < size; i++) {	
        fmc_doubleword_program(addr, buf[i]);
        addr += 8;
        fmc_flag_clear(FMC_FLAG_ENDF | FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_OPRERR | FMC_FLAG_PGSERR | FMC_FLAG_PGMERR | FMC_FLAG_PGAERR);
    }

    /* lock register FMC_CTL after the program operation */
    fmc_lock();
}

void FLASH_RegWrite (void)
{
	__disable_irq();
	
	FLASH_UserErase(8); /* page 8 */
	FLASH_UserErase(9); /* page 9 */
	FLASH_UserWrite(REG_ADDR, (uint32_t*)g_reg, sizeof(s_reg_t) / 4);
	
	__enable_irq();
}

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

	FLASH_LoadData(REG_ADDR,sizeof(s_reg_t)/4, (uint32_t)g_reg);
	g_sreg.distance = 0;
	g_sreg.rangeStatus = 0;
	g_sreg.flag = 0;
	
	if (FLASH_RegsChk(0, kRegSetCnt)) {
		modify = 1;
	}

	if (modify) {
		SetFlag(kFlagWriteReg);
	}
}
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