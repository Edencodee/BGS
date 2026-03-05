/**
 * @file    bsp_flash_simple.h
 * @brief   GD32C231 Flash 精简驱动 - A/B 双页备份
 * @note    使用最后两页（Page 62/63）存储参数
 *          - Page 62: 0x0800F800 (A 页)
 *          - Page 63: 0x0800FC00 (B 页)
 *          - 写入单位: 8 字节对齐
 * @date    2026-02-02
 */

#ifndef __BSP_FLASH_SIMPLE_H
#define __BSP_FLASH_SIMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"   
#include "gd32c2x1.h"
#include <stdint.h>

/* ==================== Flash 配置 ==================== */
#define NVM_PAGE_A              62
#define NVM_PAGE_B              63
#define NVM_PAGE_SIZE           1024
#define NVM_BASE_ADDR           0x08000000
#define NVM_PAGE_A_ADDR         (NVM_BASE_ADDR + NVM_PAGE_A * NVM_PAGE_SIZE)
#define NVM_PAGE_B_ADDR         (NVM_BASE_ADDR + NVM_PAGE_B * NVM_PAGE_SIZE)
#define FLASH_WRITE_UNIT        8               /* 必须按 8 字节写入 */

/* ==================== 底层 API ==================== */

/**
 * @brief  擦除 Flash 页
 * @param  page_index: 页索引 (62/63)
 * @retval 0: 成功, -1: 失败
 */
int32_t flash_erase_page(uint32_t page_index);

/**
 * @brief  写入 Flash（按 8 字节对齐）
 * @param  addr: 写入地址（必须 8 字节对齐）
 * @param  data: 数据缓冲区
 * @param  len: 数据长度（自动补齐到 8 字节倍数）
 * @retval 0: 成功, -1: 失败
 */
int32_t flash_write_buffer(uint32_t addr, const uint8_t *data, uint32_t len);

/**
 * @brief  读取 Flash
 * @param  addr: 读取地址
 * @param  data: 数据缓冲区
 * @param  len: 读取长度
 */
void flash_read_buffer(uint32_t addr, uint8_t *data, uint32_t len);

/**
 * @brief  简单 CRC32 计算
 * @param  data: 数据指针
 * @param  len: 数据长度
 * @retval CRC32 值
 */
uint32_t flash_crc32(const uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_FLASH_SIMPLE_H */
