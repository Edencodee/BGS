#ifndef __TASK_PARAM_H
#define __TASK_PARAM_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stdbool.h"	
/* ==================== NVM Definitions ==================== */
#define NVM_MAGIC       0x5A5AA55A
/* CRC32 = magic(4) + seq(4) + param(17) = 25byte */
#define NVM_CRC_DATA_LEN  (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(param_config_t))


/* ==================== NVM Record ==================== */
/**
 * @brief NVM storage record (40 bytes, multiple of 8)
 * @note  Structure: magic(4) + seq(4) + param(17) + crc32(4) + padding(11) = 40 bytes
 *        - magic: validity identifier (0x5A5AA55A)
 *        - seq: sequence number for A/B page rotation
 *        - param: (param_config_t)
 *        - crc32: CRC32 covering magic+seq+param
 *        - padding: to multiple of 8 bytes
 */
typedef struct {
    uint32_t magic;                 /* validity identifier */
    uint32_t seq;                   /* A/B page rotation sequence number */
    param_config_t param;           /* IC & APP parameter data */
    uint32_t crc32;                 /* CRC32 checksum */
    uint8_t  padding[11];           /* padding to 40 bytes */
} __attribute__((packed, aligned(8))) nvm_record_t;



void ParamCfgTask(void);
void ParamCfg_Init(void);
void ParamCfg_ApplyToIC(void);
bool ParamCfg_RefreshFromNvm(void);
bool ParamCfg_RestoreDefault(void);
bool ParamCfg_Save(const param_config_t *param);

bool ParamCfg_Get(param_config_t *param);
bool ParamCfg_Set(const param_config_t *param);
bool ParamCfg_SetIC(const isl700_config_t *isl700);
bool ParamCfg_SetApp(const app_config_t *app);

uint16_t ParamCfg_GetVset(void);
bool ParamCfg_SetVset(uint16_t v_set);
led_mode_t ParamCfg_GetLedMode(void);
bool ParamCfg_SetLedMode(led_mode_t mode);


#ifdef __cplusplus
}
#endif

#endif
