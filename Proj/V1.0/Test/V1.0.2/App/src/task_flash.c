#include "main.h"

/* ==================== 私有变量 ==================== */
static uint32_t s_last_seq = 0;         /* 上次保存的序列号 */
static uint32_t s_last_write_page = PARAM_PAGE_A;  /* 上次写入的页 */

/* ==================== 默认参数 ==================== */
/* Use external default config instead of local static const */
#define s_default_config g_IC_user_config

/* ==================== 私有函数 ==================== */

/**
 * @brief  Check if a record is valid based on magic and CRC32
 * @param  record: Pointer to the record
 * @retval true=valid, false=invalid
 */
static bool validate_record(const param_record_t *record)
{
    /* 1. Check magic number */
    if (record->header.magic != PARAM_MAGIC) {
        return false;
    }
    
    /* 2. Check payload length */
    if (record->header.payload_len != sizeof(param_config_t)) {
        return false;
    }
    
    /* 3. Check CRC32 */
    uint32_t calc_crc = Flash_CRC32((const uint8_t *)&record->config, 
                                         sizeof(param_config_t));
    if (calc_crc != record->header.crc32) {
        return false;
    }
    
    return true;
}

/**
 * @brief  Read a record from the specified page and validate it
 * @param  page: Page number (PARAM_PAGE_A or PARAM_PAGE_B)
 * @param  record: Output record
 * @retval true=read successfully and valid, false=invalid
 */
static bool read_record(uint32_t page, param_record_t *record)
{
    uint32_t addr = BSP_Flash_GetPageAddr(page);
    Flash_Read(addr, (uint8_t *)record, sizeof(param_record_t));
    
    return validate_record(record);
}

/**
 * @brief  Write a record to the specified page
 * @param  page: Page number
 * @param  config: Parameter configuration
 * @param  seq: Sequence number
 * @retval true=write successfully, false=write failed
 */
static bool write_record(uint32_t page, const param_config_t *config, uint32_t seq)
{
    param_record_t record;
    
    /* 1. Construct record header */
    record.header.magic = PARAM_MAGIC;
    record.header.seq = seq;
    record.header.payload_len = sizeof(param_config_t);
    record.header.reserved = 0;
    record.header.crc32 = Flash_CRC32((const uint8_t *)config, sizeof(param_config_t));
    
    /* 2. Copy parameters */
    memcpy(&record.config, config, sizeof(param_config_t));
    
    /* 3. Erase page */
    if (!Flash_ErasePage(page)) {
        return false;
    }
    
    /* 4. Write data */
    uint32_t addr = BSP_Flash_GetPageAddr(page);
    if (!Flash_Write(addr, (const uint8_t *)&record, sizeof(param_record_t))) {
        return false;
    }
    
    /* 5. Verify after write */
    if (!Flash_Verify(addr, (const uint8_t *)&record, sizeof(param_record_t))) {
        return false;
    }
    
    return true;
}

/* ==================== Public API Implementation ==================== */
void Param_Init(void)
{
    param_record_t record_a, record_b;
    bool a_valid = false;
    bool b_valid = false;
    
    /* 1. Try to read from page A */
    a_valid = read_record(PARAM_PAGE_A, &record_a);
    
    /* 2. Try to read from page B */
    b_valid = read_record(PARAM_PAGE_B, &record_b);
    
    /* 3. Select parameters based on validity */
    if (a_valid && b_valid) {
        /* Both pages are valid: choose the one with the larger sequence number 
           Sequence number (handle overflow: assume difference won't exceed 2^31) */
        int32_t diff = (int32_t)(record_a.header.seq - record_b.header.seq);
        if (diff > 0) {
            memcpy(&g_param_cfg, &record_a.config, sizeof(param_config_t));
            s_last_seq = record_a.header.seq;
            s_last_write_page = PARAM_PAGE_A;
        } else {
            memcpy(&g_param_cfg, &record_b.config, sizeof(param_config_t));
            s_last_seq = record_b.header.seq;
            s_last_write_page = PARAM_PAGE_B;
        }
    } else if (a_valid) {
        /* Only page A is valid */
        memcpy(&g_param_cfg, &record_a.config, sizeof(param_config_t));
        s_last_seq = record_a.header.seq;
        s_last_write_page = PARAM_PAGE_A;
    } else if (b_valid) {
        /* Only page B is valid */
        memcpy(&g_param_cfg, &record_b.config, sizeof(param_config_t));
        s_last_seq = record_b.header.seq;
        s_last_write_page = PARAM_PAGE_B;
    } else {
        /* Both pages are invalid: use default parameters */
        memcpy(&g_param_cfg, &s_default_config, sizeof(param_config_t));
        s_last_seq = 0;
        s_last_write_page = PARAM_PAGE_B;  /* Next write to page A */
        
        /* Optional: write to Flash immediately on first power-up */
        // Param_Save();
    }
}

const param_config_t* Param_Get(void)
{
    return (const param_config_t *)&g_param_cfg;
}

/**
 * @brief  Set parameters (overwrite entirely)
 */
void Param_Set(const param_config_t *cfg)
{
    if (cfg != NULL) {
        memcpy(&g_param_cfg, cfg, sizeof(param_config_t));
    }
}

bool Param_Save(void)
{
    /* 1. Determine target page (Ping-Pong: write to the other page) */
    uint32_t target_page = (s_last_write_page == PARAM_PAGE_A) ? 
                            PARAM_PAGE_B : PARAM_PAGE_A;
    
    /* 2. Increment sequence number */
    uint32_t new_seq = s_last_seq + 1;
    
    /* 3. Write */
    if (!write_record(target_page, &g_param_cfg, new_seq)) {
        return false;
    }
    
    /* 4. Update state */
    s_last_seq = new_seq;
    s_last_write_page = target_page;
    
    return true;
}

void Param_RestoreFactory(void)
{
    memcpy(&g_param_cfg, &s_default_config, sizeof(param_config_t));
    Param_Save();
}

/**
 * @brief  Force use of default parameters (for debugging)
 * @note   Erase Flash and load default configuration, suitable for development and debugging
 */
void Param_ForceDefault(void)
{
    /* Erase both Flash pages */
    Flash_ErasePage(PARAM_PAGE_A);
    Flash_ErasePage(PARAM_PAGE_B);
    
    /* Load default configuration */
    memcpy(&g_param_cfg, &s_default_config, sizeof(param_config_t));
    
    /* Reset internal state */
    s_last_seq = 0;
    s_last_write_page = PARAM_PAGE_B;  /* Next write to page A */
    
    /* Optional: save to Flash immediately */
    // Param_Save();
}


void Test_FlashParam(void)
{
    printf("\n=== Flash Param Test Start ===\r\n");

    /* 1. Display current parameters */
    printf("1. Current params:\r\n");
    const param_config_t *cfg = Param_Get();
    ISL700_PrintfConfig(&cfg->isl700);
    printf("   v_set=%u, v_hys=%u, led_mode=%u\r\n",
           cfg->app.v_set, cfg->app.v_hys, cfg->app.led_mode);

    /* 2. Modify parameters via Param_Set() */
    printf("2. Modify params via Param_Set():\r\n");
    param_config_t new_cfg;

    /* Incrementally modify based on current parameters (avoid overwriting isl700 part) */
    memcpy(&new_cfg, cfg, sizeof(param_config_t));
    new_cfg.app.v_set = 350;
    new_cfg.app.v_hys = 50;

    Param_Set(&new_cfg);

    /* Read back to confirm RAM has been updated */
    cfg = Param_Get();
    printf("   After Param_Set(): v_set=%u, v_hys=%u, led_mode=%u\r\n",
           cfg->app.v_set, cfg->app.v_hys, cfg->app.led_mode);

    /* 3. Save to Flash */
    printf("3. Save to Flash...\r\n");
    if (Param_Save()) {
        printf("   [OK] Param_Save() success\r\n");
    } else {
        printf("   [FAIL] Param_Save() failed\r\n");
        printf("=== Flash Param Test End ===\r\n\n");
        return;
    }

    /* 4. Simulate reboot: reload from Flash */
    printf("4. Simulate reboot: reload from Flash...\r\n");
    Param_Init();

    /* 5. Verify parameters after reload */
    printf("5. Verify params after reload:\r\n");
    cfg = Param_Get();
    printf("   v_set=%u, v_hys=%u, led_mode=%u\r\n",
           cfg->app.v_set, cfg->app.v_hys, cfg->app.led_mode);

    /* 6. Check verification result */
    if (cfg->app.v_set == 350 && cfg->app.v_hys == 50) {
        printf("   [OK] Verification PASSED\r\n");
    } else {
        printf("   [FAIL] Verification FAILED\r\n");
    }

    printf("=== Flash Param Test End ===\r\n\n");
}