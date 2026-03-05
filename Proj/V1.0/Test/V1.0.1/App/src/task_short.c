#include "main.h"


__IO bool g_isShort = false;


//调用ShortTask周期为200us
#define SHORT_TIMEOUT        	3       /* 600us */
#define SHORT_RELEASE        	10000   /* 2s */

void ShortTask(void)
{
    static int32_t s_shortCnt = 0;

    if (g_isShort == false) {
        if (RESET == FUN_GetState()) {
            /* shut down output after short circuit 600us */
            /* 短路检测阶段 */
            if (s_shortCnt < SHORT_TIMEOUT) {
                s_shortCnt++;
            }

            if (s_shortCnt >= SHORT_TIMEOUT) {
                NPN_Clr();
                g_isShort = true;
                printf("Short Circuit Detected!\r\n");
                s_shortCnt = 0;
            }
        } else {
            s_shortCnt = 0;
        }
    } else {
        /* recover after remove short circuit 2S */
        if (s_shortCnt < SHORT_RELEASE) {
            s_shortCnt++;
        }

        if (s_shortCnt >= SHORT_RELEASE) {
            g_isShort = false;
            printf("Short Circuit Released!\r\n");
            s_shortCnt = 0;
        }
    }
}


