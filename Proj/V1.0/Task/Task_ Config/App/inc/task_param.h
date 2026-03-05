#ifndef __TASK_PARAM_H
#define __TASK_PARAM_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "gd32c2x1.h"
#include "stdbool.h"
#include "param_config.h"

/**
 * @brief  初始化参数模块
 * @retval 0: 成功（从 Flash 或默认值）, -1: 失败
 * @note   首次加电或 Flash 损坏时加载出厂默认值并写入 Flash
 */
int32_t nvm_param_init(void);

/**
 * @brief  读取当前参数
 * @param  param: 输出参数指针
 * @retval 0: 成功, -1: 参数为 NULL
 */
int32_t nvm_param_get(param_config_t *param);

/**
 * @brief  修改并保存参数到 Flash
 * @param  param: 新参数指针
 * @retval 0: 成功, -1: 参数为 NULL 或保存失败
 * @note   使用 A/B 双页轮转机制，确保掉电安全
 */
int32_t nvm_param_set(const param_config_t *param);

/**
 * @brief  恢复出厂默认值
 * @retval 0: 成功, -1: 失败
 */
int32_t nvm_param_restore_default(void);


#ifdef __cplusplus
}
#endif

#endif
