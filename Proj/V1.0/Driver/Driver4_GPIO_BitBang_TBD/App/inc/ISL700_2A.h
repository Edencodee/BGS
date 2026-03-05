#ifndef __ISL700_2A_H
#define __ISL700_2A_H

#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"
#include "gd32c2x1.h"
#include "bsp_gpio.h"
#include "systick.h"
#include <stdint.h>
#include <stdbool.h>


/* CAL IO --------------------------------------------- */
#define CAL_IN_PIN           	GPIO_PIN_6
#define CAL_IN_PORT     		GPIOB
#define CAL_IN_CLK      		RCU_GPIOB

#define CAL_CLK_PIN           	GPIO_PIN_3
#define CAL_CLK_PORT     		GPIOB
#define CAL_CLK_CLK      		RCU_GPIOB

#define STS_PIN           		GPIO_PIN_0
#define STS_PORT     			GPIOB
#define STS_CLK      			RCU_GPIOB

#define CRC_RST_PIN           	GPIO_PIN_3
#define CRC_RST_PORT     		GPIOA
#define CRC_RST_CLK      		RCU_GPIOA
/* ---------------------------------------------------- */

#define ISL700_REG_DEFAULT {   	\
    .pls = 255,               	\
    .fine_gain = 62,          	\
    .coarse_gain1 = 15,        	\
    .coarse_gain2 = 1,        	\
    .sctrl_res_a = 5,         	\
    .sctrl_res_b = 5,         	\
}


/* ===== 用户可配置参数 ===== */
typedef struct
{
    /* ===== 可调参数 ===== */
    uint8_t  pls;              // [4:11]  发射脉冲幅值
    uint16_t fine_gain;        // [14:23] 细调增益 (10bit)
    uint8_t  coarse_gain1;     // [29:32] 粗调增益1

    /* ===== 可选高级参数（默认即可） ===== */
    uint8_t  coarse_gain2;     // [33:36]
    uint8_t  sctrl_res_a;      // [37:40]
    uint8_t  sctrl_res_b;      // [41:44]
} isl700_reg_t;

extern volatile isl700_reg_t g_isl700_reg;

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
bool ISL700_Write_Reg(const volatile isl700_reg_t *reg);
void ISL700_Print_Reg(const volatile isl700_reg_t *reg);

#ifdef __cplusplus
}
#endif

#endif
