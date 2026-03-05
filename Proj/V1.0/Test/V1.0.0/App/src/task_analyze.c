#include "main.h"

int16_t V_A = 0U;
int16_t V_B = 0U;
int16_t V_OUT = 0U;

void AnalyzeTask(void)
{
	/* Fast Trimmed Mean, Discard N */
    //uint16_t avgA = FastTrimmedMean(g_A_Buffer, SAMPLE_POINT, DISCARD_N);
    //uint16_t avgB = FastTrimmedMean(g_B_Buffer, SAMPLE_POINT, DISCARD_N);
    //printf("Filter A (discard %d): %d\r\n", DISCARD_N, avgA);
    //printf("Filter B (discard %d): %d\r\n", DISCARD_N, avgB);
	
    /* ADC code to voltage (mV) */
    //V_A = ((avgA * VOLTAGE) / ADC_RESOLUTION);
    //V_B = ((avgB * VOLTAGE) / ADC_RESOLUTION);
    //V_OUT = V_A - V_B;
	
//	printf("ADC Values:\r\n");
//    for(uint8_t i = 0; i < SAMPLE_POINT; i++) {
//		printf("V_A : %d\r\n", g_A_Buffer[i]);
//		printf("V_B : %d\r\n", g_B_Buffer[i]);
//    }
	
	V_A = g_A_Buffer[0];
	V_B = g_B_Buffer[0];

	
    V_OUT = V_A - V_B;
	//delay_1ms(1000);
    //printf("V_A: %d \r\n", V_A);
    //printf("V_B: %d \r\n", V_B);
	//printf("V_OUT: %d \r\n", V_OUT);
}

/* @brief Fast Trimmed Mean Filter (Integer)
 * @param buf: Input data buffer pointer
 * @param len: Data length
 * @param discard: Number of samples to discard (discard samples from both ends)
 * @return Trimmed mean result
 */
uint16_t FastTrimmedMean(const volatile uint16_t *buf, uint16_t len, uint16_t discard)
{
    if (buf == NULL || len == 0U) {
        return 0U;
    }

    if ((uint32_t)discard * 2U >= (uint32_t)len) {
        return 0U;
    }

    if (len > SAMPLE_POINT) {
        return 0U;
    }

    uint32_t sum = 0U;
    for (uint16_t i = 0U; i < len; i++) {
        sum += buf[i];
    }
	
	
	
	
	

    if (discard == 0U) {
        return (uint16_t)( sum / len);
    }

    bool used[SAMPLE_POINT] = {false};
    for (uint16_t d = 0U; d < discard; d++) {
        int16_t min_idx = -1;
        int16_t max_idx = -1;

        for (uint16_t i = 0U; i < len; i++) {
            if (!used[i]) {
                min_idx = (int16_t)i;
                break;
            }
        }

        for (int16_t i = (int16_t)len - 1; i >= 0; i--) {
            if (!used[i]) {
                max_idx = i;
                break;
            }
        }

        if (min_idx < 0 || max_idx < 0) {
            break;
        }

        for (uint16_t i = 0U; i < len; i++) {
            if (!used[i] && buf[i] < buf[(uint16_t)min_idx]) {
                min_idx = (int16_t)i;
            }
        }

        for (uint16_t i = 0U; i < len; i++) {
            if (!used[i] && (int16_t)i != min_idx && buf[i] > buf[(uint16_t)max_idx]) {
                max_idx = (int16_t)i;
            }
        }

        if (min_idx == max_idx) {
            for (uint16_t i = 0U; i < len; i++) {
                if (!used[i] && (int16_t)i != min_idx) {
                    max_idx = (int16_t)i;
                    break;
                }
            }
        }

        if (min_idx >= 0 && max_idx >= 0 && min_idx != max_idx) {
            used[(uint16_t)min_idx] = true;
            used[(uint16_t)max_idx] = true;
            sum -= (uint32_t)buf[(uint16_t)min_idx];
            sum -= (uint32_t)buf[(uint16_t)max_idx];
        }
    }

    uint16_t cnt = (uint16_t)(len - 2U * discard);
    return (uint16_t)(sum / cnt);
}

