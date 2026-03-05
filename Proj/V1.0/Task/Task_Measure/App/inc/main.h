#ifndef MAIN_H
#define MAIN_H



/* C Standard Library Include */
#include "gd32c2x1.h"
#include <stdio.h>
#include "systick.h"

/* Output Include */
#include "SEGGER_RTT.h"

/* BSP Include */
#include "bsp_gpio.h"
#include "bsp_adc.h"
#include "bsp_exit.h"
#include "bsp_flash_simple.h"

/*IC Include */
#include "ISL700_2A.h"

/*Task Include */
#include "task_init.h"
#include "task_meas.h"
#include "task_output.h"
#include "task_short.h"
#include "task_param.h"
#include "task_btn.h"
#include "task_led.h"
#include "task_analyze.h"

/* Global Header */
#include "param_config.h"

extern __IO bool g_isSampleDone;
extern __IO bool g_startADCSampling;
extern __IO bool g_isSysTickInt;

#endif /* MAIN_H */
