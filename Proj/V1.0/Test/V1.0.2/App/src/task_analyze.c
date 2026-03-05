#include "main.h"

uint16_t V_A = 0U;
uint16_t V_B = 0U;
uint16_t V_OUT = 0U;

bool g_isVoutReady = false;
bool g_isVoutValid = false;

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

static uint16_t adc_to_voltage(uint16_t adc_val)
{
  return (uint16_t)(((uint32_t)adc_val * VOLTAGE) / ADC_RESOLUTION);
}


bool ProcessVoutAverage(uint16_t vout_sample, uint16_t *final_vout)
{
  static uint16_t vout_samples[VOUT_AVERAGE_COUNT];
  static uint8_t sample_count = 0U;

  /* N=1 No averaging */
  if (VOUT_AVERAGE_COUNT == 1U) {
    *final_vout = vout_sample;
    return true;
  }

  /* N>1 Accumulate samples */
  if (sample_count < VOUT_AVERAGE_COUNT) {
    vout_samples[sample_count++] = vout_sample;
  }

  /* When N samples are reached, calculate the average */
  if (sample_count >= VOUT_AVERAGE_COUNT) {
    uint32_t sum = 0U;
    uint8_t i;

    for (i = 0U; i < VOUT_AVERAGE_COUNT; i++) {
      sum += vout_samples[i];
    }

    *final_vout = (uint16_t)(sum / VOUT_AVERAGE_COUNT);
    sample_count = 0U;  /* Reset counter, start next accumulation */
    return true;
  }

  /* Samples not yet accumulated, return false */
  return false;
}

void AnalyzeTask(void)
{
  uint16_t avgA = FastTrimmedMean((const volatile uint16_t *)g_A_Buffer, SAMPLE_POINT, DISCARD_N);
  uint16_t avgB = FastTrimmedMean((const volatile uint16_t *)g_B_Buffer, SAMPLE_POINT, DISCARD_N);

  V_A = adc_to_voltage(avgA);
  V_B = adc_to_voltage(avgB);

  /* Differential calculation: only valid when V_B >= V_A */
  if (V_B >= V_A) {
    uint16_t vout_raw = V_B - V_A;
    uint16_t vout_averaged = 0U;

    g_isVoutValid = true;

    /* Accumulate N valid samples and average */
    if (ProcessVoutAverage(vout_raw, &vout_averaged)) {
      V_OUT = vout_averaged;
      g_isVoutReady = true;  /* Average complete, data ready */
    } else {
      g_isVoutReady = false; /* Still accumulating, data not ready */
    }
  } else {
    V_OUT = 0U;
    g_isVoutValid = false;
    g_isVoutReady = true;  /* Invalid sample immediately ready (output NO_OBJECT) */
    
    // printf("Invalid Sample: V_B < V_A (V_A=%d, V_B=%d)\r\n", V_A, V_B);
  }
}




	
	
	


