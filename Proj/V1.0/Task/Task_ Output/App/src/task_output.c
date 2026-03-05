#include "task_output.h"
#include "bsp_gpio.h"

/* ==================== 内部状态 ==================== */
static bool s_presence = false;

/* ==================== Output 任务 API ==================== */

void OutputTask_Init(void)
{
    s_presence = false;
    NPNClr();
}

void OutputTask_SetPresence(bool presence)
{
    s_presence = presence;
}

bool OutputTask_GetPresence(void)
{
    return s_presence;
}

void OutputTask(void)
{
	if (s_presence) {
		NPNSet();
	} else {
		NPNClr();
	}
}