#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include "n32l40x.h"
#include "wtdint.h"

#include "systick.h"
#include "SEGGER_RTT.h"

#include "filter.h"
#include "task.h"
#include "task_flash.h"
#include "task_short.h"
#include "task_output.h"
#include "task_btn.h"
#include "task_measure.h"
#include "task_led.h"
#include "modbus.h"
#include "atsp.h"

#include "user_flash.h"
#include "user_gpio.h"
#include "user_uart.h"
#include "user_i2c.h"
#include "user_tim.h"
#include "user_fdog.h"

#include "vl53lx_api.h"
#include "vl53lx_platform_user_data.h"

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
