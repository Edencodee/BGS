#include "main.h"

static uint16_t s_btn_task_tick = 0;

void Task_Init(void)
{
	//ParamCfg_Init();
    //LedTask_Init();
    BtnTask_Init();
}

/* sysTick interrupt call back -------------------------------------------------------------- */
void SysTickTask (void)
{
    s_btn_task_tick++;
    if(s_btn_task_tick >= 10) { // 10ms tick
        s_btn_task_tick = 0;
        BtnTask();
    }
}
