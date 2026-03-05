#include "main.h"

/* Call period: 200us */
#define SHORT_TIMEOUT        	3       /* 600us: consecutive counts to confirm short */
#define SHORT_RELEASE        	10000   /* 2s: counts to recover after short removed */

void ShortTask(void)
{
    static int32_t s_shortCnt = 0;

    if (g_outputState != SHORT_CIRCUIT) {
        /* Detecting phase */
        if (RESET == FUN_GetState()) {
            if (s_shortCnt < SHORT_TIMEOUT) {
                s_shortCnt++;
            }
            if (s_shortCnt >= SHORT_TIMEOUT) {
                g_outputState = SHORT_CIRCUIT;  /* LedTask will clear NPN */
                printf("Short Circuit Detected!\r\n");
                s_shortCnt = 0;
            }
        } else {
            s_shortCnt = 0;
        }
    } else {
        /* Recovery phase: wait 2s after short removed */
        if (s_shortCnt < SHORT_RELEASE) {
            s_shortCnt++;
        }
        if (s_shortCnt >= SHORT_RELEASE) {
            g_outputState = NO_OBJECT;  /* Release SHORT_CIRCUIT, resume normal operation */
            printf("Short Circuit Released!\r\n");
            s_shortCnt = 0;
        }
    }
}


