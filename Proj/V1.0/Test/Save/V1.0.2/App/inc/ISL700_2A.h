#ifndef __ISL700_2A_H
#define __ISL700_2A_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "crc_8.h"
#include "param_config.h"
#include "stdbool.h"


/* CAL control macros --------------------------------- */
#define CAL_IN_HIGH   			    gpio_bit_set(CAL_IN_PORT, CAL_IN_PIN)
#define CAL_IN_LOW   				gpio_bit_reset(CAL_IN_PORT, CAL_IN_PIN)
#define CAL_CLK_HIGH  			    gpio_bit_set(CAL_CLK_PORT, CAL_CLK_PIN)
#define CAL_CLK_LOW  			    gpio_bit_reset(CAL_CLK_PORT, CAL_CLK_PIN)
#define CRC_RST_HIGH    			gpio_bit_set(CRC_RST_PORT, CRC_RST_PIN)
#define CRC_RST_LOW     			gpio_bit_reset(CRC_RST_PORT, CRC_RST_PIN)
#define READ_STS                    gpio_input_bit_get(STS_PORT, STS_PIN)
/* ---------------------------------------------------- */

/* ===== API ===== */
uint8_t IC_CalcCRC(const volatile isl700_config_t *r);
bool ISL700_Write_Reg(const volatile isl700_config_t *cfg);

#ifdef __cplusplus
}
#endif

#endif
