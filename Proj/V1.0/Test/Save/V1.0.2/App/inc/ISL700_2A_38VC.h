#ifndef __ISL700_2A_38VC_H__
#define __ISL700_2A_38VC_H__

#include "main.h"
#include "crc_zh.h"
#include "param_config.h"

/* CAL control macros --------------------------------- */
#define CAL_IN_HIGH                gpio_bit_set(CAL_IN_PORT, CAL_IN_PIN)
#define CAL_IN_LOW                 gpio_bit_reset(CAL_IN_PORT, CAL_IN_PIN)
#define CAL_CLK_HIGH               gpio_bit_set(CAL_CLK_PORT, CAL_CLK_PIN)
#define CAL_CLK_LOW                gpio_bit_reset(CAL_CLK_PORT, CAL_CLK_PIN)
#define CRC_RST_HIGH               gpio_bit_set(CRC_RST_PORT, CRC_RST_PIN)
#define CRC_RST_LOW                gpio_bit_reset(CRC_RST_PORT, CRC_RST_PIN)
#define READ_STS                   gpio_input_bit_get(STS_PORT, STS_PIN)
/* ---------------------------------------------------- */

extern uint8_t FuseCheck_flag;

void ISL700_WriteConfig(volatile isl700_config_t *cfg);    // 写ISL700_2A程序
void ISL700_PrintfConfig(const volatile isl700_config_t *cfg); //打印配置参数
void IC_PinConfig(void); // ISL700_2A引脚配置
void IC_ExternalClkInit(void);
void ISL700_Init(volatile isl700_config_t *cfg); // ISL700_2A初始化配置




#endif
