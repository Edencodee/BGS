/**
 * @file    nvm_param_simple.h
 * @brief   NVM 参数管理 - 精简版
 * @note    直接存储统一的 param_config_t 结构体
 *          支持 A/B 双页掉电保护和出厂恢复
 *          
 *          方案说明：
 *          - 采用 A/B 双页备份（Page 62/63）
 *          - 直接存储统一的 param_config_t 结构体
 *          - 使用 seq 序号区分新旧页
 *          - CRC32 校验确保数据完整性
 *          - 4 个精简 API：init/get/set/restore_default
 *          
 * @date    2026-02-03
 */

#ifndef __NVM_PARAM_SIMPLE_H
#define __NVM_PARAM_SIMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "param_config.h"
#include <stdint.h>

/* ==================== NVM 存储记录 ==================== */

/**
 * @brief NVM 存储记录（40 字节，8 的倍数）
 * @note  结构：magic(4) + seq(4) + param(20) + crc32(4) + padding(8) = 40 字节
 *        - magic: 有效性标识（0x5A5AA55A）
 *        - seq: 序列号，用于 A/B 页轮转
 *        - param: 实际参数数据（param_config_t）
 *        - crc32: 覆盖 magic+seq+param 的校验值
 *        - padding: 补齐到 8 字节倍数
 */
typedef struct {
    uint32_t magic;                 /* 有效性标识 */
    uint32_t seq;                   /* 页序号（用于 A/B 轮转） */
    param_config_t param;           /* 参数数据 */
    uint32_t crc32;                 /* CRC32 校验 */
    uint8_t  padding[8];            /* 补齐到 40 字节 */
} __attribute__((packed, aligned(8))) nvm_record_t;

/* ==================== 精简 API ==================== */

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

#endif /* __NVM_PARAM_SIMPLE_H */
