#ifndef __PARAM_CONFIG_H
#define __PARAM_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


/* ==================== ISL700 Config Struct ==================== */
typedef struct {
    /* ===== ISL700 Regs（15字节）===== */
    uint8_t  fuse_check_floating;   /* SR[50:57] = 0，固定不修改 */
    uint8_t  fuse_check_low;        /* SR[0:3]   = 9，固定不修改 */
    uint8_t  pls;                   /* SR[4:11]  = 255，发射脉冲幅值 */
    uint8_t  no_used;               /* SR[12:13] = 1，保留未使用 */
    uint16_t fine_gain;             /* SR[14:23] = 62，细调增益（10bit，code越大增益越大） */
    uint8_t  gain_select;           /* SR[24]    = 0，增益选择（0=x1, 1=x1.5） */
    uint8_t  fuse_check_middle;     /* SR[25:28] = 10，固定不修改 */
    uint8_t  coarse_gain1;          /* SR[29:32] = 15，粗调增益1（code越大增益越大） */
    uint8_t  coarse_gain2;          /* SR[33:36] = 1，粗调增益2（建议设置为0） */
    uint8_t  sctrl_res_a;           /* SR[37:40] = 5，前级增益a（建议≤8） */
    uint8_t  sctrl_res_b;           /* SR[41:44] = 5，前级增益b（建议≤8） */
    uint8_t  sctrl_internal;        /* SR[45]    = 1，电阻选择（0=外部, 1=内部） */
    uint8_t  fuse_check_high;       /* SR[46:49] = 6，固定不修改 */
    uint8_t crc8;
    
} __attribute__((packed)) isl700_config_t;


/* ==================== APP Config Struct ==================== */
typedef struct {
    uint16_t v_set;                 /* Set voltage (mV) */
	uint16_t v_hys;					/* Hysteresis(mV) */
    uint8_t  led_mode;              /* LED mode: 0=off, 1=on */
} __attribute__((packed)) app_config_t;

/* ================== Unified Complete Parameter Struct =================== */

/**
 * @brief Complete parameter configuration structure
 * @note  Includes ISL700 configuration parameters + APP parameters
 *        Total size: 18 bytes (15 + 3)
 *        Only the ISL700 part (isl700_config_t) is used when configuring the IC
 */
typedef struct {
    isl700_config_t isl700;         /* ISL700 configuration parameters (15 bytes) */
    app_config_t    app;            /* APP parameters (6 bytes) */
} __attribute__((packed)) param_config_t;

/* ==================== User Defaults ==================== */
/* @version: 1.0  @last_modified: 2026-03-02 - UPDATE THIS LINE AFTER PARAMETER CHANGES */

#define ISL700_CONFIG_USER {     	\
    .fuse_check_floating = 0,       \
    .fuse_check_low      = 9,       \
    .pls                 = 255,     \
    .no_used             = 1,       \
    .fine_gain           = 62,      \
    .gain_select         = 0,       \
    .fuse_check_middle   = 10,      \
    .coarse_gain1        = 15,      \
    .coarse_gain2        = 1,       \
    .sctrl_res_a         = 5,       \
    .sctrl_res_b         = 5,       \
    .sctrl_internal      = 1,       \
    .fuse_check_high     = 6,       \
    .crc8                = 0,		\
}

#define APP_CONFIG_USER {        	\
    .v_set               = 400,    \
    .v_hys               = 50,     \
    .led_mode            = 1,       \
}

#define PARAM_CONFIG_USER {      	\
    .isl700 = ISL700_CONFIG_USER,	\
    .app    = APP_CONFIG_USER,   	\
}

/* ==================== Factory Defaults ==================== */

#define ISL700_CONFIG_DEFAULT {     \
    .fuse_check_floating = 0,       \
    .fuse_check_low      = 9,       \
    .pls                 = 255,     \
    .no_used             = 1,       \
    .fine_gain           = 62,      \
    .gain_select         = 0,       \
    .fuse_check_middle   = 10,      \
    .coarse_gain1        = 15,      \
    .coarse_gain2        = 1,       \
    .sctrl_res_a         = 5,       \
    .sctrl_res_b         = 5,       \
    .sctrl_internal      = 1,       \
    .fuse_check_high     = 6,       \
    .crc8                = 0,		\
}

#define APP_CONFIG_DEFAULT {        \
    .v_set               = 400,    \
    .v_hys               = 50,     \
    .led_mode            = 1,       \
}

#define PARAM_CONFIG_DEFAULT {      \
    .isl700 = ISL700_CONFIG_DEFAULT,\
    .app    = APP_CONFIG_DEFAULT,   \
}

/* ==================== External Configuration Constants ==================== */
/**
 * @brief Default configuration constants (defined in param_config.c)
 * @note  Use these constants instead of macros for better dependency tracking
 */
extern const param_config_t g_IC_user_config;
extern const param_config_t g_IC_factory_config;

#ifdef __cplusplus
}
#endif

#endif /* __PARAM_CONFIG_H */

