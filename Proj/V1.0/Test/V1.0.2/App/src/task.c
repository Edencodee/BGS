#include "main.h"
#include "task_flash.h"

static uint16_t s_btn_task_tick = 0;

void Task_Init(void)
{
    /* 初始化参数系统（从Flash加载或使用默认） */
    //Param_Init();
    
    //LedTask_Init();
    //BtnTask_Init();
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
