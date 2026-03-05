#include "main.h"
#include "debug_config.h"

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

  /* N=1 时，直接输出，无平均 */
  if (VOUT_AVERAGE_COUNT == 1U) {
    *final_vout = vout_sample;
    return true;
  }

  /* N>1 时，积累样本 */
  if (sample_count < VOUT_AVERAGE_COUNT) {
    vout_samples[sample_count++] = vout_sample;
  }

  /* 达到 N 个样本，计算平均 */
  if (sample_count >= VOUT_AVERAGE_COUNT) {
    uint32_t sum = 0U;
    uint8_t i;

    for (i = 0U; i < VOUT_AVERAGE_COUNT; i++) {
      sum += vout_samples[i];
    }

    *final_vout = (uint16_t)(sum / VOUT_AVERAGE_COUNT);
    sample_count = 0U;  /* 重置计数器，开始下一轮积累 */
    return true;
  }

  /* 样本未积累够，返回 false */
  return false;
}

void AnalyzeTask(void)
{
  uint16_t avgA = FastTrimmedMean((const volatile uint16_t *)g_A_Buffer, SAMPLE_POINT, DISCARD_N);
  uint16_t avgB = FastTrimmedMean((const volatile uint16_t *)g_B_Buffer, SAMPLE_POINT, DISCARD_N);

  V_A = adc_to_voltage(avgA);
  V_B = adc_to_voltage(avgB);

  /* VERBOSE模式：打印原始ADC数据 */
  DEBUG_ADC_VERBOSE("avgA=%d, avgB=%d\r\n", avgA, avgB);

  /* 差分计算：仅当 V_B >= V_A 时有效 */
  if (V_B >= V_A) {
    uint16_t vout_raw = V_B - V_A;
    uint16_t vout_averaged = 0U;

    g_isVoutValid = true;

    /* 积累 N 个有效样本并平均 */
    if (ProcessVoutAverage(vout_raw, &vout_averaged)) {
      V_OUT = vout_averaged;
      g_isVoutReady = true;  /* 平均完成，数据就绪 */
      
      /* 调试：打印平均完成的电压值 */
      DEBUG_ANALYZE("V_A=%d, V_B=%d, V_OUT=%d mV [AVERAGED]\r\n", V_A, V_B, V_OUT);
      
      /* 统计：记录有效样本 */
      Debug_UpdateStatistics(true, true);
    } else {
      g_isVoutReady = false; /* 还在积累中，数据未就绪 */
      DEBUG_ANALYZE("Accumulating... (raw=%d mV)\r\n", vout_raw);
    }
  } else {
    V_OUT = 0U;
    g_isVoutValid = false;
    g_isVoutReady = true;  /* 无效样本立即就绪（输出 NO_OBJECT）*/
    
    /* 调试：限频打印无效样本（每100次打印一次，避免刷屏） */
    DEBUG_THROTTLE(DEBUG_PRINT_INTERVAL_DEFAULT, 
                   "[WARN] Invalid Sample: V_B < V_A (V_A=%d, V_B=%d)\r\n", V_A, V_B);
    
    /* 统计：记录无效样本 */
    Debug_UpdateStatistics(false, true);
  }
}




	
	
	


