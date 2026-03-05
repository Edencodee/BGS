#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H

/**
 * @file    bsp_flash.h
 * @brief   GD32C231 Flash 通用驱动层
 * @note    - 纯驱动实现，无业务逻辑限制
 *          - 可擦任意页（由调用者负责页号有效性）
 *          - 可写任意地址（8字节对齐）
 *          - 提供CRC32校验工具
 */

#ifdef __cplusplus
extern "C" {
#endif
 
#include "gd32c2x1.h"
#include <stdint.h>
#include <stdbool.h>

/* ==================== Flash 物理参数 ==================== */
#define FLASH_PAGE_SIZE         1024U
#define FLASH_BASE_ADDR         0x08000000UL
#define FLASH_WRITE_UNIT        8U           /* GD32C231 写入单位：8字节 */
#define FLASH_TOTAL_PAGES       64U          /* GD32C231C8: 64KB / 1KB */

/* ==================== 驱动层 API ==================== */

/**
 * @brief  擦除指定页
 * @param  page_index: 页号（0-63）
 * @retval true=成功, false=失败
 * @note   调用者需确保页号有效，驱动层不做业务判断
 */
bool Flash_ErasePage(uint32_t page_index);

/**
 * @brief  写入数据到Flash
 * @param  addr: 目标地址（必须8字节对齐）
 * @param  data: 源数据缓冲区
 * @param  len: 数据长度（自动补齐到8字节倍数）
 * @retval true=成功, false=失败（地址未对齐或写入错误）
 */
bool Flash_Write(uint32_t addr, const uint8_t *data, uint32_t len);

/**
 * @brief  从Flash读取数据
 * @param  addr: 源地址
 * @param  data: 目标缓冲区
 * @param  len: 读取字节数
 */
void Flash_Read(uint32_t addr, uint8_t *data, uint32_t len);

/**
 * @brief  写后校验（推荐在写入后调用）
 * @param  addr: Flash地址
 * @param  data: 期望数据
 * @param  len: 数据长度
 * @retval true=校验通过, false=校验失败
 */
bool Flash_Verify(uint32_t addr, const uint8_t *data, uint32_t len);

/**
 * @brief  计算CRC32校验和
 * @param  data: 数据缓冲区
 * @param  len: 数据长度
 * @retval CRC32值
 */
uint32_t Flash_CRC32(const uint8_t *data, uint32_t len);

/**
 * @brief  获取页地址
 * @param  page_index: 页号
 * @retval 页起始地址
 */
static inline uint32_t BSP_Flash_GetPageAddr(uint32_t page_index)
{
    return FLASH_BASE_ADDR + (page_index * FLASH_PAGE_SIZE);
}

#ifdef __cplusplus
}
#endif

#endif /* __BSP_FLASH_H */
