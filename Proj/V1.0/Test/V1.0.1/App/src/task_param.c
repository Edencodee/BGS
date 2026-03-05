#include "main.h"

/* ============== Factory default parameter ================*/
static const param_config_t FACTORY_DEFAULT = PARAM_CONFIG_DEFAULT;
static const param_config_t USER_CONFIG = PARAM_CONFIG_USER;

/* ==================== Internal State ==================== */
static nvm_record_t g_current;          /* RAM cache */

/* ============== NVM Internal Functions ============== */
static int32_t NVM_InitParam(void);
static int32_t NVM_GetParam(param_config_t *param);
static int32_t NVM_SetParam(const param_config_t *param);
static int32_t NVM_RestoreDefault(void);


/* ======= Parameter Configuration Explicit Interface ======== */

/**
 * @brief  Parameter initialize and apply to IC
 * @retval true: success, false: failure
 */
void ParamCfgTask(void)
{
	ParamCfg_Init();
	//ParamCfg_ApplyToIC();
}

/**
 * @brief  Initialize NVM parameter
 * @retval true: success, false: failure
 * @note   Load parameters from Flash to memory cache
 */
void ParamCfg_Init(void)
{
	/* Initialize NVM parameter module */
	NVM_InitParam();
	/* Load NVM parameters to cache */
	ParamCfg_RefreshFromNvm();
}

/**
 * @brief Apply current parameters to ISL700 IC
 * @note   Only pass ISL700 parameters, without app parameters
 */
void ParamCfg_ApplyToIC(void)
{
	ISL700_Write_Reg(&g_param_cfg.isl700);
}

bool ParamCfg_RefreshFromNvm(void)
{
	return (NVM_GetParam((param_config_t *)&g_param_cfg) == 0);
}

/**
 * @brief  Restore factory default parameters
 * @note   Restore default values and save to Flash
 */
bool ParamCfg_RestoreDefault(void)
{
	/* Restore factory default values */
	if (NVM_RestoreDefault() != 0) {
		return false;
	}
	/* Reload parameters */
	if (!ParamCfg_RefreshFromNvm()) {
		return false;
	}
	return true;
}

bool ParamCfg_Save(const volatile param_config_t *param)
{
	param_config_t temp_param;

	if (param == NULL) {
		return false;
	}

	temp_param = *param;
    
	/* Save to Flash */
	if (NVM_SetParam(&temp_param) != 0) {
		return false;
	}
    
	/* Refresh from Flash */
	g_param_cfg = temp_param;
	return true;
}

/* ========= Parameter Configuration Get & Set ========== */

bool ParamCfg_SetIC(const isl700_config_t *isl700)
{
	if (isl700 == NULL) {
		return false;
	}
	g_param_cfg.isl700 = *isl700;
	return true;
}

bool ParamCfg_SetApp(const app_config_t *app)
{
	if (app == NULL) {
		return false;
	}
	g_param_cfg.app = *app;
	return true;
}

void ParamCfg_Print(volatile param_config_t *cfg)
{
    if (cfg == NULL) {
        return;
    }
    printf("=====================\r\n");    
    printf("=== IC Registers ===\r\n");
    printf(" FUSE_L:         %u\r\n", (unsigned)cfg->isl700.fuse_check_low);
    printf(" PLS:            %u\r\n", (unsigned)cfg->isl700.pls);
    printf(" NO_USED:        %u\r\n", (unsigned)cfg->isl700.no_used);
    printf(" FINE_GAIN:      %u\r\n", (unsigned)cfg->isl700.fine_gain);
    printf(" GAIN_SEL:       %u\r\n", (unsigned)cfg->isl700.gain_select);
    printf(" FUSE_M:         %u\r\n", (unsigned)cfg->isl700.fuse_check_middle);
    printf(" CG1:            %u\r\n", (unsigned)cfg->isl700.coarse_gain1);
    printf(" CG2:            %u\r\n", (unsigned)cfg->isl700.coarse_gain2);
    printf(" SRA:            %u\r\n", (unsigned)cfg->isl700.sctrl_res_a);
    printf(" SRB:            %u\r\n", (unsigned)cfg->isl700.sctrl_res_b);
    printf(" SCTRL_INT:      %u\r\n", (unsigned)cfg->isl700.sctrl_internal);
    printf(" FUSE_H:         %u\r\n", (unsigned)cfg->isl700.fuse_check_high);
    printf(" FUSE_F:         %u\r\n", (unsigned)cfg->isl700.fuse_check_floating);

    printf("==== APP Params ====\r\n");
    printf(" V_SET:          %u\r\n", (unsigned)cfg->app.v_set);
    printf(" LED:            %u\r\n", (unsigned)cfg->app.led_mode);

    printf("=====================\r\n");
}

/* ============ APP Parameter Get & Set ============ */
uint16_t ParamCfg_GetVset(void)
{
	return g_param_cfg.app.v_set;
}

bool ParamCfg_SetVset(uint16_t v_set)
{
	g_param_cfg.app.v_set = v_set;
	return ParamCfg_Save(&g_param_cfg);
}

led_mode_t ParamCfg_GetLedMode(void)
{
	return (g_param_cfg.app.led_mode == 1U) ?  LIGHT_ON : DARK_ON ;
}

bool ParamCfg_SetLedMode(led_mode_t mode)
{
	g_param_cfg.app.led_mode = (uint8_t)mode;
	return ParamCfg_Save(&g_param_cfg);
}

/* ==================== Internal Functions ==================== */
/**
 * @brief  load parameter from Flash (A/B dual-page)
 * @param  rec: output record pointer
 * @retval 0: success (at least one page valid), -1: both pages invalid
 * @note   A/B dual-page mechanism: select the page with valid and largest seq
 */
static int32_t load_from_flash(nvm_record_t *rec)
{
	nvm_record_t rec_a, rec_b;
    
	/* Read pages A and B */
	flash_read_buffer(NVM_PAGE_A_ADDR, (uint8_t *)&rec_a, sizeof(rec_a));
	flash_read_buffer(NVM_PAGE_B_ADDR, (uint8_t *)&rec_b, sizeof(rec_b));
    
	/* Verify magic and CRC of page A */
	int valid_a = 0;
	if (rec_a.magic == NVM_MAGIC) {
		uint32_t crc = flash_crc32((uint8_t *)&rec_a, NVM_CRC_DATA_LEN);
		if (crc == rec_a.crc32) {
			valid_a = 1;
		}
	}
    
	/* Verify magic and CRC of page B */
	int valid_b = 0;
	if (rec_b.magic == NVM_MAGIC) {
		uint32_t crc = flash_crc32((uint8_t *)&rec_b, NVM_CRC_DATA_LEN);
		if (crc == rec_b.crc32) {
			valid_b = 1;
		}
	}
    
	/* Select the valid page with the latest seq */
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
    
	return -1;  /* both pages invalid */
}

/**
 * @brief  Save parameters to Flash (A/B dual-page rotation)
 * @param  rec: record to save
 * @retval 0: success, -1: failure
 * @note   Erase the oldest page (smaller seq) and write new data
 */
static int32_t save_to_flash(const nvm_record_t *rec)
{
	nvm_record_t rec_a, rec_b;
    
	/* Read seq numbers of both pages */
	flash_read_buffer(NVM_PAGE_A_ADDR, (uint8_t *)&rec_a, sizeof(rec_a));
	flash_read_buffer(NVM_PAGE_B_ADDR, (uint8_t *)&rec_b, sizeof(rec_b));
    
	/* Select smaller seq for overwrite */
	uint32_t target_page;
	uint32_t target_addr;
    
	if (rec_a.magic != NVM_MAGIC || rec_a.seq <= rec_b.seq) {
		target_page = NVM_PAGE_A;
		target_addr = NVM_PAGE_A_ADDR;
	} else {
		target_page = NVM_PAGE_B;
		target_addr = NVM_PAGE_B_ADDR;
	}
    
	/* Erase target page */
	if (flash_erase_page(target_page) != 0) {
		return -1;
	}
    
	/* Write new record */
	if (flash_write_buffer(target_addr, (const uint8_t *)rec, sizeof(*rec)) != 0) {
		return -1;
	}
    
	return 0;
}

/* ==================== API Implementation ==================== */

/**
 * @brief  Initialize parameter module
 */
static int32_t NVM_InitParam(void)
{
	/* Load parameters from Flash */
	if (load_from_flash(&g_current) == 0) {
		return 0;
	}
    
	/* Flash invalid, initialize to factory defaults */
	g_current.magic = NVM_MAGIC;
	g_current.seq = 0;
	g_current.param = USER_CONFIG;
	memset(g_current.padding, 0xFF, sizeof(g_current.padding));
    
	/* Calculate CRC32 (covering magic+seq+param) */
	g_current.crc32 = flash_crc32((uint8_t *)&g_current, NVM_CRC_DATA_LEN);
    
	return save_to_flash(&g_current);
}

/**
 * @brief  Read current parameters (from RAM cache)
 * @param  param: output parameter buffer
 * @retval 0: success, -1: param is NULL
 */
static int32_t NVM_GetParam(param_config_t *param)
{
	if (param == NULL) {
		return -1;
	}
	*param = g_current.param;
	return 0;
}

/**
 * @brief  Modify and save parameters to Flash
 * @param  param: new parameters
 * @retval 0: success, -1: param is NULL or save failed
 * @note   A/B dual-page ensures power-off safety, at least one page is always valid
 */
static int32_t NVM_SetParam(const param_config_t *param)
{
	if (param == NULL) {
		return -1;
	}
    
	/* Update parameters in RAM cache */
	g_current.param = *param;
	g_current.seq++;
    
	/* Recalculate CRC32 (covering magic+seq+param) */
	g_current.crc32 = flash_crc32((uint8_t *)&g_current, NVM_CRC_DATA_LEN);
    
	return save_to_flash(&g_current);
}

/**
 * @brief  Restore factory defaults
 */
static int32_t NVM_RestoreDefault(void)
{
	return NVM_SetParam(&FACTORY_DEFAULT);
}




