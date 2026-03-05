#include "main.h"
#include "debug_config.h"

__IO output_state_t g_outputState = NO_OBJECT;

/**
 * @brief  输出判定任务
 * @note   特性：
 *         1. 回差机制：v_on=400mV 吸合，v_off=350mV 释放（抗抖动）
 *         2. 抗闪断：连续 N 次无效样本才判定超量程（N=OOR_ENTER_COUNT）
 *         3. 快速恢复：OUT_OF_RANGE 收到有效样本立即重判定
 *         4. 状态保持：短暂无效时保持上次输出状态
 */
void OutputTask(void)
{
    const uint16_t v_on  = V_SET;
    const uint16_t v_hys = V_HYS;
    const uint16_t v_off = (v_on > v_hys) ? (uint16_t)(v_on - v_hys) : 0U;

    /* 无效样本连续计数器（在 OutputTask 维护，因为 invalid 也会触发该任务） */
    static uint8_t invalid_count = 0U;

    /* 无效样本阈值 */
    const uint8_t N_invalid = (uint8_t)OOR_ENTER_COUNT;

	/* ============ 0) 短路保护优先级最高 ============ */
    if (g_outputState == SHORT_CIRCUIT) {
        NPN_Clr();
        printf("SHORT CIRCUIT\r\n");
        return;
    }


    /* ============ 1) 无效/有效样本入口处理 ============ */
    if (!g_isVoutValid) {
        /* 防止计数器溢出 */
        if (invalid_count < 255U) {
            invalid_count++;
        }

        /* 1.1 未达阈值：保持上次输出（抗闪断） */
        if (invalid_count < N_invalid) {
            if (g_outputState == OBJECT_PRESENT) {
                NPN_Set();
            } else {
                NPN_Clr();
            }
            /* 限频打印：每100次打印一次，避免刷屏 */
            DEBUG_THROTTLE(DEBUG_PRINT_INTERVAL_DEFAULT, 
                          "[OUTPUT] Invalid sample, keeping state: %s (count=%d/%d)\r\n",
                          g_outputState == OBJECT_PRESENT ? "PRESENT" : "NO_OBJ",
                          invalid_count, N_invalid);
            return;
        }

        /* 1.2 连续 invalid 达到 N：判定超量程/无效区 */
        if (g_outputState != OUT_OF_RANGE) {
            g_outputState = OUT_OF_RANGE;
            g_debug_stats.out_of_range_count++;
            g_debug_stats.state_transitions++;
            DEBUG_WARN("OUT_OF_RANGE: invalid_count=%d\r\n", invalid_count);
        }
        NPN_Clr();
        return;
    }

    /* 有效样本到来：清零计数器 */
    invalid_count = 0U;

    /* ============ 2) OUT_OF_RANGE 快速恢复（无延迟） ============ */
    if (g_outputState == OUT_OF_RANGE) {
        /* 注意：此时 V_OUT 是平均后的稳定值（g_isVoutReady=true 才会进入） */
        output_state_t new_state = (V_OUT >= v_on) ? OBJECT_PRESENT : NO_OBJECT;
        if (new_state != g_outputState) {
            DEBUG_INFO("Recovered from OUT_OF_RANGE -> %s (V_OUT=%d)\r\n",
                      new_state == OBJECT_PRESENT ? "PRESENT" : "NO_OBJECT", V_OUT);
            g_debug_stats.state_transitions++;
        }
        g_outputState = new_state;
    }

    /* ============ 3) NO_OBJECT <-> OBJECT_PRESENT 回差状态机 ============ */
    output_state_t prev_state = g_outputState;
    
    switch (g_outputState) {

    case OBJECT_PRESENT:
        /* 已吸合：只有 <= v_off 才释放（带回差） */
        if (V_OUT <= v_off) {
            g_outputState = NO_OBJECT;
        }
        break;

    case NO_OBJECT:
		/* 已释放：只有 >= v_on 才吸合（带回差） */
        if (V_OUT >= v_on) {
            g_outputState = OBJECT_PRESENT;
        }
        break;
    default:
	    g_outputState = NO_OBJECT;
        break;
        
    }
    
    /* 调试：检测状态切换 */
    if (prev_state != g_outputState) {
        g_debug_stats.state_transitions++;
        if (g_outputState == OBJECT_PRESENT) {
            g_debug_stats.object_present_count++;
            DEBUG_OUTPUT("State: NO_OBJECT -> OBJECT_PRESENT (V_OUT=%d >= v_on=%d)\r\n", V_OUT, v_on);
        } else if (g_outputState == NO_OBJECT) {
            DEBUG_OUTPUT("State: OBJECT_PRESENT -> NO_OBJECT (V_OUT=%d <= v_off=%d)\r\n", V_OUT, v_off);
        }
    }

    /* ============ 4) 输出驱动 ============ */
    if (g_outputState == OBJECT_PRESENT) {
        NPN_Set();
        DEBUG_THROTTLE(DEBUG_PRINT_INTERVAL_DEFAULT, "[OUTPUT] OBJECT_PRESENT\r\n");
    } else {
        /* NO_OBJECT / OUT_OF_RANGE 都是关断 */
        NPN_Clr();
        DEBUG_THROTTLE(DEBUG_PRINT_INTERVAL_DEFAULT, "[OUTPUT] NO_OBJECT/OOR\r\n");
    }
}



