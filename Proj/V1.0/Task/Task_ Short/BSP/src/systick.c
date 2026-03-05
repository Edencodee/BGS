#include "gd32c2x1.h"
#include "systick.h"

volatile static uint32_t delay;

/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if(SysTick_Config(SystemCoreClock / 1000U)) {
        /* capture error */
        while(1) {
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

/*!
    \brief      delay a time in milliseconds
    \param[in]  count: count in milliseconds
    \param[out] none
    \retval     none
*/
void delay_1ms(uint32_t count)
{
    delay = count;

    while(0U != delay) {
    }
}

/*!
    \brief      delay decrement
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay_decrement(void)
{
    if(0U != delay) {
        delay--;
    }
}

/*!
    \brief      delay a time in microseconds
    \param[in]  count: delay time in microseconds
    \param[out] none
    \retval     none
    \note       基于SysTick计数器实现的精确微秒延迟
                适用于需要高精度短延迟的场合（如GPIO时序控制）
*/
void delay_1us(uint32_t count)
{
    uint32_t ticks = count * (SystemCoreClock / 1000000U);  // 计算总时钟周期数
    uint32_t start = SysTick->VAL;                          // 记录起始计数值
    uint32_t reload = SysTick->LOAD + 1;                    // SysTick重载值+1

    while (ticks > 0)
    {
        uint32_t cur = SysTick->VAL;                     // 读取当前计数值
        uint32_t elapsed;                                // 已经过的时钟周期数

        // SysTick是向下计数的，需要处理回卷情况
        if (start >= cur) {
            elapsed = start - cur;                       // 未发生回卷
        } else {
            elapsed = start + reload - cur;              // 发生了回卷
        }

        // 更新剩余ticks并重置起始点
        if (elapsed > 0) {
            if (elapsed >= ticks) {
                break;                                   // 延迟完成
            }
            ticks -= elapsed;                            // 减去已经过的周期
            start = cur;                                 // 更新起始点
        }
    }
}
