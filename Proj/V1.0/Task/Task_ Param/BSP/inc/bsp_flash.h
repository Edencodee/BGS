#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H


/**
 * @file    bsp_flash.h
 * @brief   GD32C231 Flash - A/B 双页备份
 * @note    使用最后两页（Page 62/63）存储参数
 *          - Page 62: 0x0800F800 (A 页)
 *          - Page 63: 0x0800FC00 (B 页)
 *          - 写入单位: 8 字节对齐
 */

#ifdef __cplusplus
extern "C" {
#endif
 
#include "gd32c2x1.h"
#include <stdint.h>

/* ==================== Flash Config ==================== */
#define NVM_PAGE_A              62
#define NVM_PAGE_B              63
#define NVM_PAGE_SIZE           1024
#define NVM_BASE_ADDR           0x08000000
#define NVM_PAGE_A_ADDR         (NVM_BASE_ADDR + NVM_PAGE_A * NVM_PAGE_SIZE)
#define NVM_PAGE_B_ADDR         (NVM_BASE_ADDR + NVM_PAGE_B * NVM_PAGE_SIZE)
#define FLASH_WRITE_UNIT        8            /* 必须按 8 字节写入 */


int32_t flash_erase_page(uint32_t page_index);
int32_t flash_write_buffer(uint32_t addr, const uint8_t *data, uint32_t len);
void flash_read_buffer(uint32_t addr, uint8_t *data, uint32_t len);
uint32_t flash_crc32(const uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_FLASH_H */
