#include "main.h"
#include "task_init.h"
#include "task_param.h"
#include "ISL700_2A.h"
#include <stdio.h>
#include <string.h>

__IO bool g_isSampleDone = true;
__IO bool g_startADCSampling = false;
__IO bool g_isSysTickInt = false;


void BSP_Init();

int main(void)
{
	BSP_Init();
    Task_Init();  // Init BSP and parameter service, auto-config ISL700
    while (1) {
        exti_software_interrupt_enable(PLS_SO_EXTI_LINE);
        delay_1ms(1000);
        if (g_isSysTickInt) {
			g_isSysTickInt = false;
			//SysTickCallBack();
		}

        if(g_startADCSampling == true) {
            g_startADCSampling = false;
            MeasTask();
        } 

        if(g_isSampleDone == true) {
            AnalyzeTask();  
        }   

    }
}

void BSP_Init(void)
{
    systick_config();
    GPIO_UserInit();
    PLS_SO_InterruptInit();  /* initialize external interrupt on PA6 (PLS_SO) */
    ADC_UserInit();          /* initialize ADC for regular mode */
}

int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return (ch);
}
