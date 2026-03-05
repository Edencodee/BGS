#include "task_analyze.h"

void AnalyzeTask(void)
{
    printf("V_A Values:\r\n");
    for(uint8_t i = 0; i < SAMPLE_POINT; i++) {
        printf("%d\r\n", g_A_Buffer[i]);
    }
    printf("V_B Values:\r\n");
    for(uint8_t i = 0; i < SAMPLE_POINT; i++) {
        printf("%d\r\n", g_B_Buffer[i]);
    }
}
