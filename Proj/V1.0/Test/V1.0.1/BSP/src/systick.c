#include "gd32c2x1.h"
#include "systick.h"
#include "stdio.h"

volatile static uint32_t delay;
volatile uint32_t tick_count;

/* variables for microsecond timing */
volatile static uint32_t tick_start_us;
volatile static uint32_t systick_start_us;

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


/* measure execution time */
void start_timing_ms(void)
{
    tick_count = 0;
}

uint32_t stop_timing_ms(void)
{
    return tick_count;
}

void print_timing_ms(uint32_t time)
{
    printf("Time : %d ms\r\n",time);
}


void start_timing_us(void)
{
    tick_start_us = tick_count;                  /* 记录开始时的毫秒计数 */
    systick_start_us = SysTick->VAL;             /* 记录开始时的SysTick计数值 */
}


uint32_t stop_timing_us(void)
{
    uint32_t tick_end = tick_count;              /* 当前毫秒计数 */
    uint32_t systick_end = SysTick->VAL;         /* 当前SysTick计数值 */
    
    uint32_t systick_reload = SysTick->LOAD + 1; /* SysTick重载值 */
    uint32_t ms_elapsed;
    uint32_t us_elapsed;
    
 
    ms_elapsed = tick_end - tick_start_us;
    
    /* 计算SysTick内的微秒差值 */
    /* SysTick是向下计数，需要处理计数器回卷 */
    uint32_t systick_us_per_reload = 1000U;      /* 每个reload周期为1ms = 1000us */
    uint32_t systick_elapsed;
    
    if (systick_start_us >= systick_end) {
        /* 未发生回卷 */
        systick_elapsed = systick_start_us - systick_end;
    } else {
        /* 发生了回卷 */
        systick_elapsed = systick_start_us + systick_reload - systick_end;
    }
    
    /* 将SysTick周期转换为微秒 */
    /* systick_elapsed / systick_reload * 1000us = systick_elapsed * 1000 / systick_reload */
    us_elapsed = (systick_elapsed * 1000U) / systick_reload;
    
    /* 总微秒数 = 毫秒部分 + SysTick部分 */
    return (ms_elapsed * 1000U) + us_elapsed;
}


void print_timing_us(uint32_t time)
{
    uint32_t ms = time / 1000U;
    uint32_t us = time % 1000U;
    printf("Time : %d ms %d us (Total: %d us)\r\n", ms, us, time);
}
