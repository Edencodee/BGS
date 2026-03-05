#ifndef __TASK_H
#define __TASK_H

#ifdef cplusplus
extern "C" {
#endif

#include "main.h"

#if DOUBLE_ELE
	typedef double ele_t;
#else
	typedef float ele_t;
#endif

	void AppInit(void);
	void RecvIdleCallBack(void* userData, uint16_t len);
	void HandleCSIFrame(uint16_t* p);
	void SysTickCallBack(void);
	void Timer2UpdateCallBack(void);
	bool LoopCntInc (int32_t *cnt, int32_t max);
	bool CntInc (int32_t *cnt, int32_t max);

#ifdef cplusplus
}
#endif

#endif
