#ifndef __PARAM_CONFIG_H
#define __PARAM_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


/* ==================== ISL700 配置参数结构体 ==================== */

/**
 * @brief ISL700 寄存器配置参数
 * @note  仅包含 ISL700 IC 的寄存器字段（14字节）
 *        用于 ISL700_Write_Reg() 函数
 *        不包含应用层参数
 */
typedef struct {
    /* ===== ISL700 寄存器字段（14字节）===== */
    
    /* 固定不修改的寄存器 */
    uint8_t  fuse_check_low;        /* SR[0:3]   = 9，固定不修改 */
    
    /* 可调参数 */
    uint8_t  pls;                   /* SR[4:11]  = 255，发射脉冲幅值 */
    uint8_t  no_used;               /* SR[12:13] = 1，保留未使用 */
    uint16_t fine_gain;             /* SR[14:23] = 62，细调增益（10bit，code越大增益越大） */
    uint8_t  gain_select;           /* SR[24]    = 0，增益选择（0=x1, 1=x1.5） */
    
    /* 固定不修改的寄存器 */
    uint8_t  fuse_check_middle;     /* SR[25:28] = 10，固定不修改 */
    
    /* 可调参数 */
    uint8_t  coarse_gain1;          /* SR[29:32] = 15，粗调增益1（code越大增益越大） */
    
    /* 可选高级参数（默认即可） */
    uint8_t  coarse_gain2;          /* SR[33:36] = 1，粗调增益2（建议设置为0） */
    uint8_t  sctrl_res_a;           /* SR[37:40] = 5，前级增益a（建议≤8） */
    uint8_t  sctrl_res_b;           /* SR[41:44] = 5，前级增益b（建议≤8） */
    uint8_t  sctrl_internal;        /* SR[45]    = 1，电阻选择（0=外部, 1=内部） */
    
    /* 固定不修改的寄存器 */
    uint8_t  fuse_check_high;       /* SR[46:49] = 6，固定不修改 */
    uint8_t  fuse_check_floating;   /* SR[50:57] = 0，固定不修改 */
    
} __attribute__((packed)) isl700_config_t;

/* ==================== 应用层参数结构体 ==================== */

/**
 * @brief 应用层参数
 * @note  不影响 ISL700 IC 配置，仅用于应用逻辑
 */
typedef struct {
    uint16_t v_set;                 /* 比较阈值(mV) */
    uint8_t  led_mode;              /* 工作灯模式: 0=暗通, 1=亮通 */
} __attribute__((packed)) app_config_t;

/* ==================== 统一的完整参数结构体 ==================== */

/**
 * @brief 完整的参数配置结构体
 * @note  包含 ISL700 配置参数 + 应用层参数
 *        总大小：17字节（14 + 3）
 *        
 *        使用场景：
 *        - Flash 存储：存储整个结构体
 *        - NVM 管理：管理整个结构体
 *        - 内存缓存：缓存整个结构体
 *        
 *        但配置 IC 时只使用 ISL700 部分（isl700_config_t）
 */
typedef struct {
    isl700_config_t isl700;         /* ISL700 配置参数（14字节）*/
    app_config_t    app;            /* 应用层参数（6字节）*/
} __attribute__((packed)) param_config_t;

/* ==================== Factory Defaults ==================== */

#define ISL700_CONFIG_DEFAULT {     \
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
    .fuse_check_floating = 0,       \
}

#define APP_CONFIG_DEFAULT {        \
    .v_set               = 1200,    \
    .led_mode            = 0,       \
}

#define PARAM_CONFIG_DEFAULT {      \
    .isl700 = ISL700_CONFIG_DEFAULT,\
    .app    = APP_CONFIG_DEFAULT,   \
}

#ifdef __cplusplus
}
#endif

#endif /* __PARAM_CONFIG_H */

