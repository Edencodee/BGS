#ifndef MAIN_H
#define MAIN_H



/* Minimal core includes */
#include "gd32c2x1.h"

#include "bsp.h"
#include "task.h"

#include "ISL700_2A_38VC.h"
#include "param_config.h"

#include "task_led.h"
#include "task_btn.h"
#include "task_meas.h"
#include "task_short.h"
#include "task_output.h"
#include "task_analyze.h"

#include "systick.h"
#include "bsp_adc.h"
#include "bsp_gpio.h"
#include "bsp_exit.h"
#include "bsp_flash.h"



#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


extern __IO bool g_isSampleDone;
extern __IO bool g_isStartSampling;
extern __IO bool g_isSysTickInt;
extern __IO bool g_isA_Done;   /* A路DMA采样完成标志 */
extern __IO bool g_isB_Done;   /* B路DMA采样完成标志 */
extern volatile param_config_t g_param_cfg;


void Debug_Print(void);
#endif /* MAIN_H */
