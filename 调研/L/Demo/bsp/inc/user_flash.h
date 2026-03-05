#ifndef __USER_FLASH_H
#define __USER_FLASH_H

#ifdef cplusplus
extern "C" {
#endif

#include "n32l40x.h"
#include <stdbool.h>

#define PAGE_SIZE			0x800		/* 2k											*/

										/* boot 16k, sector  0~ 7, 0x8000000~0x8003fff	*/
#define REG_ADDR			0x8004000ul	/* REG   4k, sector  8~ 9, 0x8004000~0x8004fff	*/
#define PARA_ADDR			0x8005000ul /* PARA  4k, sector 10~11, 0x8005000~0x8005fff	*/
#define COEF_ADDR			0x8006000ul /* COEF  4k, sector 12~13, 0x8006000~0x8006fff	*/
#define APP_ADDR			0x8007000ul	/* app, 0x8007000~0x801ffff 					*/

#define APP_SIZE			0x18F000			

#if APP
	typedef enum {
		kFlagInit = 0,
		kFlagOutState,
		kFlagShort,
		kFlagWriteReg,
	} flag_t;

	/* reg define --------------------------------- */
	typedef enum {
		kLonBgs = 0,
		kDonFgs,
		kLonFgs,
		kDonBgs,
		kStateCnt
	} state_t;
	
	typedef struct {
		uint16_t state;
		uint16_t p1orp2;
		uint16_t cali1p1;
		uint16_t cali1p2;
		uint16_t cali2p2;	
		uint16_t res5_19[15];
		
		uint16_t distance;
		uint16_t rangeStatus;
		uint16_t btnLed[3];
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
		kRegBtnYled,	
		kRegFlag,
		kRegTempCnt
	} reg_idx_t;
	
	extern __IO s_reg_t g_sreg;
	extern __IO uint16_t *g_reg;
	extern s_reg_t sregDef;
	extern uint16_t *regDef;
	extern s_reg_t sregMax;
	extern uint16_t *regMax;
	extern s_reg_t sregMin;
	extern uint16_t *regMin;
	extern __IO uint16_t* g_rom;
#endif	
	/* -------------------------------------------- */
	void FLASH_UserErase (uint32_t pageNum);
	void FLASH_UserWrite (uint32_t addr, uint32_t *buf, uint16_t size);
#if APP			
	void FLASH_RegWrite  (void);
	void FLASH_RegLoadChk  (void);
	void FLASH_RegReinit  (void);
	
	bool ChkFlag (flag_t flag);
	void SetFlag (flag_t flag);
	void ClrFlag (flag_t flag);
#endif	
#ifdef cplusplus
}
#endif

#endif
