#include "main.h"


/* ==================== Default Configuration Constants ==================== */

/**
 * @brief Default user configuration (used when no Flash data available)
 * @note  Modify parameters here and use Build (not just Rebuild)
 *        Changes will be automatically detected by Keil
 */
const param_config_t g_IC_user_config = {
    .isl700 = {
        .fuse_check_floating = 0,
        .fuse_check_low      = 9,
        .pls                 = 255,
        .no_used             = 1,
        .fine_gain           = 62,
        .gain_select         = 0,
        .fuse_check_middle   = 10,
        .coarse_gain1        = 15,
        .coarse_gain2        = 1,     
        .sctrl_res_a         = 5,
        .sctrl_res_b         = 5,
        .sctrl_internal      = 1,
        .fuse_check_high     = 6,
        .crc8                = 0,
    },
    .app = {
        .v_set               = 400,
        .v_hys               = 50,
        .led_mode            = 1,
    }
};

/**
 * @brief Factory default configuration
 */
const param_config_t g_IC_factory_config = {
    .isl700 = {
        .fuse_check_floating = 0,
        .fuse_check_low      = 9,
        .pls                 = 255,
        .no_used             = 1,
        .fine_gain           = 62,
        .gain_select         = 0,
        .fuse_check_middle   = 10,
        .coarse_gain1        = 15,
        .coarse_gain2        = 1,       /* Factory default value */
        .sctrl_res_a         = 5,
        .sctrl_res_b         = 5,
        .sctrl_internal      = 1,
        .fuse_check_high     = 6,
        .crc8                = 0,
    },
    .app = {
        .v_set               = 400,
        .v_hys               = 50,
        .led_mode            = 1,
    }
};
