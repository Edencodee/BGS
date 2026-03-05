#include "task_output.h"
#include "task_short.h"
#include "bsp_gpio.h"

/* ==================== 内部状态 ==================== */
static bool s_presence = false;
__IO bool g_outState = false;

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
	/* 1. 输出配置 可能需要常开常闭切换 TBD */

	/* 2. 检测存在的逻辑 */


	/* 3. 短路保护 */
    if (g_isShort) {
        NPNClr();  
        return;
    }
	/* 获取输出状态 */
	if (s_presence) {
		NPNSet();
	} else {
		NPNClr();
	}
}
