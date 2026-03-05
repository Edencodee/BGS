#include "main.h"
#include "task_init.h"
#include "task_param.h"
#include "ISL700_2A.h"
#include <stdio.h>
#include <string.h>

void BSP_Init();

int main(void)
{
	BSP_Init();
    Task_Init();  // Init BSP and parameter service, auto-config ISL700
    while (1) {
        delay_1ms(1000);
    }
}

void BSP_Init(void)
{
    systick_config();
    GPIO_UserInit();
}

int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}
