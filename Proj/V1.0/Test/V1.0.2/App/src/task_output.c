#include "main.h"

__IO output_state_t g_outputState = NO_OBJECT;

void OutputTask(void)
{
    /* Use parameters from Flash (can be modified via Param_Set()) */
    const uint16_t v_on  = g_param_cfg.app.v_set;
    const uint16_t v_hys = g_param_cfg.app.v_hys;
    const uint16_t v_off = (v_on > v_hys) ? (uint16_t)(v_on - v_hys) : 0U;

    /* Invalid sample counter ,for judge ORR */
    static uint8_t invalid_count = 0U;

    /* Invalid sample threshold */
    const uint8_t N_invalid = (uint8_t)OOR_ENTER_COUNT;

	/* ============ 0) SHORT_CIRCUIT highest priority, return immediately ============ */
    if (g_outputState == SHORT_CIRCUIT) {
        return;
    }

    /* ============ 1) Invalid/Valid sample entry processing ============ */
    if (!g_isVoutValid) {
        /* Prevent counter overflow */
        if (invalid_count < 255U) {
            invalid_count++;
        }

        /* 1.1 Below threshold: keep last output (anti-flash) */
        if (invalid_count < N_invalid) {
            /* Keep last state (g_outputState unchanged) */
            //printf("%s (Last State)\r\n", (g_outputState == OBJECT_PRESENT) ? "OBJECT PRESENT" : "NO OBJECT");
            return;
        }

        /* 1.2 Consecutive invalid samples reach N: judge out-of-range/invalid area */
        g_outputState = OUT_OF_RANGE;
		printf("OUT OF RANGE\r\n");
        return;
    }

    /* Valid sample arrived: reset counter */
    invalid_count = 0U;

    /* ============ 2) OUT_OF_RANGE quick recovery (no delay) ============ */
    if (g_outputState == OUT_OF_RANGE) {
        /* 注意：此时 V_OUT 是平均后的稳定值（g_isVoutReady=true 才会进入） */
        g_outputState = (V_OUT >= v_on) ? OBJECT_PRESENT : NO_OBJECT;
    }

    /* ============ 3) NO_OBJECT <-> OBJECT_PRESENT  ============ */
    switch (g_outputState) {

    case OBJECT_PRESENT:
        /* Already engaged: only <= v_off will release (with hysteresis) */
        if (V_OUT <= v_off) {
            g_outputState = NO_OBJECT;
        }
        break;

    case NO_OBJECT:
		/* Already released: only >= v_on will engage (with hysteresis) */
        if (V_OUT >= v_on) {
            g_outputState = OBJECT_PRESENT;
        }
        break;
    default:
	    g_outputState = NO_OBJECT;
        break;
        
    }

    /* ============ 4) Output state printing (NPN controlled by LedTask) ============ */
    if (g_outputState == OBJECT_PRESENT) {
		printf("OBJECT PRESENT\r\n");
    } else {
		printf("NO OBJECT\r\n");
    }
}



