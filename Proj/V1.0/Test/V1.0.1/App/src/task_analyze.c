#include "main.h"

int16_t V_A = 0U;
int16_t V_B = 0U;
int16_t V_OUT = 0U;

static void sort_int16(int16_t *buf, uint16_t len)
{
  uint16_t i;
  for (i = 1U; i < len; i++) {
    int16_t key = buf[i];
    int16_t j = (int16_t)i - 1;
    while (j >= 0 && buf[j] > key) {
      buf[j + 1] = buf[j];
      j--;
    }
    buf[j + 1] = key;
  }
}

uint16_t FastTrimmedMean(const volatile uint16_t *buf, uint16_t len, uint16_t discard)
{
  uint16_t i;
  uint32_t sum = 0U;
  uint16_t start;
  uint16_t end;
  uint16_t count;
  uint16_t copy_len = len;
  uint16_t tmp[SAMPLE_POINT];

  if (buf == NULL || len == 0U) {
    return 0U;
  }

  if (copy_len > SAMPLE_POINT) {
    copy_len = SAMPLE_POINT;
  }

  for (i = 0U; i < copy_len; i++) {
    tmp[i] = buf[i];
  }

  for (i = 1U; i < copy_len; i++) {
    uint16_t key = tmp[i];
    int16_t j = (int16_t)i - 1;
    while (j >= 0 && tmp[j] > key) {
      tmp[j + 1] = tmp[j];
      j--;
    }
    tmp[j + 1] = key;
  }

  if (discard * 2U >= copy_len) {
    return 0U;
  }

  start = discard;
  end = (uint16_t)(copy_len - discard);
  count = (uint16_t)(end - start);

  for (i = start; i < end; i++) {
    sum += tmp[i];
  }

  return (uint16_t)(sum / count);
}

void AnalyzeTask(void)
{
  static int16_t vout_valid[VOUT_VALID_LEN];
  static uint8_t vout_count = 0U;
  static int16_t vout_final = 0;

  uint16_t avgA = FastTrimmedMean((const volatile uint16_t *)g_A_Buffer,
            SAMPLE_POINT, DISCARD_N);
  uint16_t avgB = FastTrimmedMean((const volatile uint16_t *)g_B_Buffer,
            SAMPLE_POINT, DISCARD_N);

  V_A = (int16_t)(((uint32_t)avgA * VOLTAGE) / ADC_RESOLUTION);
  V_B = (int16_t)(((uint32_t)avgB * VOLTAGE) / ADC_RESOLUTION);

  {
    int16_t v_out = (int16_t)(V_A - V_B);
    if (v_out >= 0) {
      if (vout_count < VOUT_VALID_LEN) {
        vout_valid[vout_count++] = v_out;
      }
    }
  }

  if (vout_count >= VOUT_VALID_LEN) {
    int16_t sorted[VOUT_VALID_LEN];
    uint8_t i;
    uint8_t win = (uint8_t)VOUT_CLUSTER_WINDOW;
    uint8_t best_start = 0U;
    int16_t best_range = 0x7FFF;
    uint32_t sum = 0U;

    for (i = 0U; i < vout_count; i++) {
      sorted[i] = vout_valid[i];
    }

    sort_int16(sorted, vout_count);

    if (win > vout_count) {
      win = vout_count;
    }

    // choose the tightest window as the most concentrated cluster
    for (i = 0U; i <= (uint8_t)(vout_count - win); i++) {
      int16_t range = (int16_t)(sorted[i + win - 1U] - sorted[i]);
      if (range < best_range) {
        best_range = range;
        best_start = i;
      }
    }

    for (i = 0U; i < win; i++) {
      sum += (uint16_t)sorted[best_start + i];
    }
    vout_final = (int16_t)(sum / win);
    vout_count = 0U;
  }

  delay_1ms(10);
  V_OUT = vout_final;
  printf("V_OUT: %d \r\n", V_OUT);
}


	
	
	


