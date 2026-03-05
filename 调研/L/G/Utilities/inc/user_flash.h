#ifndef __USER_FLASH_H
#define __USER_FLASH_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32c2x1.h"
#include <stdbool.h>

	typedef enum {
		kFlagInit = 0,
		kFlagOutState,
		kFlagShort,
		kFlagWriteReg,
		kFlagCloseOut,
	} flag_t;
	
		typedef enum {
		kLonBgs = 0,
		kDonBgs,
	} state_t;
		
		typedef struct {
		uint16_t state;
		uint16_t cali1p1;
		uint16_t cali1p2;
		uint16_t cali2p2;	
		uint16_t res5_19[15];
		
		uint16_t distance;
		uint16_t rangeStatus;
		uint16_t btnLed[2];
		uint16_t flag;
		uint16_t res24_39[16];
	} s_reg_t;

	typedef enum {
		kRegState = 0,
		kReg1or2P,
		kRegCali1P1,
		kRegCali1P2,	
		kRegCali2P2,
		kRegSetCnt, // even number
		
		kRegDistance,
		kRegRangeStatus,
		kRegBtnRled = 20,
		kRegBtnGled,	
		kRegFlag,
		kRegTempCnt
	} reg_idx_t;
	
	extern __IO s_reg_t g_sreg;
	extern __IO uint16_t *g_reg;
	
void fmc_erase_pages(void);
void FLASH_UserErase(uint32_t pageNum);		
void fmc_program(void);
void FLASH_UserWrite (uint32_t addr, uint32_t *buf, uint16_t size);	
void FLASH_RegWrite (void);

bool ChkFlag (flag_t flag);
void SetFlag (flag_t flag);
void ClrFlag (flag_t flag);
	
#ifdef cplusplus
}
#endif

#endif
