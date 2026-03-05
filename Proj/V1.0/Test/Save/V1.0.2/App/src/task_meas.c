#include "main.h"


void MeasureTask(void)
{
    /* 最先关闭 EXTI 重入窗口：防止 SwitchBuffer 执行期间 EXTI 再次触发
     * 更新 g_plsState 并设 g_isStartSampling，导致本轮 DMA 传输中途换缓冲区 */
    g_isSampleDone = false;

    /* g_plsState 已在 EXTI ISR 中于中断触发时刻锁存，此处直接使用 */
    if (g_plsState == kHigh) {
        /* 即将采样 B 路：立即清除旧的 B Done 标志，
         * 防止上一轮残留的 g_isB_Done=true 与本轮新采A的
         * g_isA_Done=true 共同形成跨轮错误配对触发分析 */
        g_isB_Done = false;
        ADC_SwitchBuffer((int16_t *)g_B_Buffer);
    } else {
        /* 即将采样 A 路：同理清除旧的 A Done 标志 */
        g_isA_Done = false;
        ADC_SwitchBuffer((int16_t *)g_A_Buffer);
    }

    ADC_StartSampling();
}


