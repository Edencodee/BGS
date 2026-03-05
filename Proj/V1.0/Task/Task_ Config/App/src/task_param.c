#include "task_param.h"
#include "nvm_param_simple.h"
#include "bsp_flash_simple.h"
#include <string.h>

#define NVM_MAGIC       0x5A5AA55A

/* CRC32 覆盖的有效数据长度（不包括crc32字段和padding）*/
/* = magic(4) + seq(4) + param(20) = 28字节 */
#define NVM_CRC_DATA_LEN  (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(param_config_t))

/* ==================== 出厂默认参数 ==================== */
const param_config_t FACTORY_DEFAULT = PARAM_CONFIG_DEFAULT;

/* ==================== 内部状态 ==================== */
static nvm_record_t g_current;  /* RAM 缓存 */

/* ==================== 内部函数 ==================== */

/**
 * @brief  从 Flash 加载有效参数记录
 * @param  rec: 输出的记录指针
 * @retval 0: 成功（至少一页有效）, -1: 两页都无效
 * @note   A/B 双页机制：选择有效且 seq 最大的页
 */
static int32_t load_from_flash(nvm_record_t *rec)
{
	nvm_record_t rec_a, rec_b;
    
	/* 读取 A 页和 B 页 */
	flash_read_buffer(NVM_PAGE_A_ADDR, (uint8_t *)&rec_a, sizeof(rec_a));
	flash_read_buffer(NVM_PAGE_B_ADDR, (uint8_t *)&rec_b, sizeof(rec_b));
    
	/* 验证 A 页的 magic 和 CRC */
	int valid_a = 0;
	if (rec_a.magic == NVM_MAGIC) {
		uint32_t crc = flash_crc32((uint8_t *)&rec_a, NVM_CRC_DATA_LEN);
		if (crc == rec_a.crc32) {
			valid_a = 1;
		}
	}
    
	/* 验证 B 页的 magic 和 CRC */
	int valid_b = 0;
	if (rec_b.magic == NVM_MAGIC) {
		uint32_t crc = flash_crc32((uint8_t *)&rec_b, NVM_CRC_DATA_LEN);
		if (crc == rec_b.crc32) {
			valid_b = 1;
		}
	}
    
	/* 选择有效且序号最新的页 */
	if (valid_a && valid_b) {
		*rec = (rec_a.seq > rec_b.seq) ? rec_a : rec_b;
		return 0;
	} else if (valid_a) {
		*rec = rec_a;
		return 0;
	} else if (valid_b) {
		*rec = rec_b;
		return 0;
	}
    
	return -1;  /* 两页都无效或损坏 */
}

/**
 * @brief  保存参数到 Flash（A/B 双页轮转）
 * @param  rec: 要保存的记录
 * @retval 0: 成功, -1: 失败
 * @note   擦除最旧页（seq 较小），写入新数据
 */
static int32_t save_to_flash(const nvm_record_t *rec)
{
	nvm_record_t rec_a, rec_b;
    
	/* 读取两页的 seq 号 */
	flash_read_buffer(NVM_PAGE_A_ADDR, (uint8_t *)&rec_a, sizeof(rec_a));
	flash_read_buffer(NVM_PAGE_B_ADDR, (uint8_t *)&rec_b, sizeof(rec_b));
    
	/* 选择 seq 较小的页进行覆盖 */
	uint32_t target_page;
	uint32_t target_addr;
    
	if (rec_a.magic != NVM_MAGIC || rec_a.seq <= rec_b.seq) {
		target_page = NVM_PAGE_A;
		target_addr = NVM_PAGE_A_ADDR;
	} else {
		target_page = NVM_PAGE_B;
		target_addr = NVM_PAGE_B_ADDR;
	}
    
	/* 擦除目标页 */
	if (flash_erase_page(target_page) != 0) {
		return -1;
	}
    
	/* 写入新记录 */
	if (flash_write_buffer(target_addr, (const uint8_t *)rec, sizeof(*rec)) != 0) {
		return -1;
	}
    
	return 0;
}

/* ==================== API 实现 ==================== */

/**
 * @brief  初始化参数模块
 * @retval 0: 成功, -1: 失败
 */
int32_t nvm_param_init(void)
{
	/* 尝试从 Flash 加载有效参数 */
	if (load_from_flash(&g_current) == 0) {
		return 0;
	}
    
	/* Flash 无效，初始化为出厂默认值 */
	g_current.magic = NVM_MAGIC;
	g_current.seq = 0;
	g_current.param = FACTORY_DEFAULT;
	memset(g_current.padding, 0xFF, sizeof(g_current.padding));
    
	/* 计算 CRC32（覆盖 magic+seq+param） */
	g_current.crc32 = flash_crc32((uint8_t *)&g_current, NVM_CRC_DATA_LEN);
    
	return save_to_flash(&g_current);
}

/**
 * @brief  读取当前参数（从 RAM 缓存）
 * @param  param: 输出参数缓冲区
 * @retval 0: 成功, -1: 参数为 NULL
 */
int32_t nvm_param_get(param_config_t *param)
{
	if (param == NULL) {
		return -1;
	}
	*param = g_current.param;
	return 0;
}

/**
 * @brief  修改并保存参数到 Flash
 * @param  param: 新参数
 * @retval 0: 成功, -1: 参数为 NULL 或保存失败
 * @note   A/B 双页确保掉电安全，至少一页总是有效
 */
int32_t nvm_param_set(const param_config_t *param)
{
	if (param == NULL) {
		return -1;
	}
    
	/* 更新 RAM 缓存中的参数 */
	g_current.param = *param;
	g_current.seq++;
    
	/* 重新计算 CRC32（覆盖 magic+seq+param） */
	g_current.crc32 = flash_crc32((uint8_t *)&g_current, NVM_CRC_DATA_LEN);
    
	return save_to_flash(&g_current);
}

/**
 * @brief  恢复出厂默认值
 */
int32_t nvm_param_restore_default(void)
{
	return nvm_param_set(&FACTORY_DEFAULT);
}


