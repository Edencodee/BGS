#ifndef __TASK_FLASH_H
#define __TASK_FLASH_H

/**
 * @file    task_flash.h
 * @brief   参数Flash存储应用层（双页Ping-Pong机制）
 * @note    - 使用最后两页（Page 62/63）做A/B备份
 *          - 上电自动加载参数到RAM
 *          - Magic + CRC32校验
 *          - 序列号判断最新
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "param_config.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define PARAM_PAGE_A            62U
#define PARAM_PAGE_B            63U
#define PARAM_MAGIC             0x48503650UL  /* "HP6P" */

extern param_config_t g_param_cfg;  /* RAM param */


/**
 * @brief  Flash存储记录头
 */
typedef struct {
    uint32_t magic;                 /* 魔数：0x48503650 ("HP6P") */
    uint32_t seq;                   /* 序列号（递增，用于判断新旧） */
    uint16_t payload_len;           /* 有效载荷长度 */
    uint16_t reserved;              /* 保留字段 */
    uint32_t crc32;                 /* 整个payload的CRC32 */
} __attribute__((packed)) param_header_t;


typedef struct {
    param_header_t   header;
    param_config_t   config;        /* RAM param */
} __attribute__((packed)) param_record_t;


void Param_Init(void);
const param_config_t* Param_Get(void);
void Param_Set(const param_config_t *cfg);
bool Param_Save(void);
void Param_RestoreFactory(void);
//Debug API
void Param_ForceDefault(void);  
void Test_FlashParam(void);


#ifdef __cplusplus
}
#endif

#endif /* __TASK_FLASH_H */
