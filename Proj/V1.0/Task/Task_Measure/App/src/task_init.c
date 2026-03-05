#include "task_init.h"
#include "task_param.h"
#include "task_led.h"
#include "task_btn.h"
#include "ISL700_2A.h"

/* ==================== 内部状态 ==================== */
static param_config_t g_current_param;  /* 当前运行参数缓存 */

/* ==================== BSP 初始化 ==================== */


void Task_Init(void)
{
    /* 初始化 LED 任务（上电绿灯常亮） */
    LedTask_Init();

    /* 初始化按键任务（默认 Light_ON） */
    BtnTask_Init();

    /* 初始化参数服务 */
    ParamSrv_Init();
    
    /* 将参数应用到 ISL700 芯片 */
    ParamSrv_ApplyToIC();
}

/* ==================== 参数服务 API 实现 ==================== */

/**
 * @brief  初始化参数服务
 * @retval true: 成功, false: 失败
 * @note   从 Flash 加载参数到内存缓存
 */
bool ParamSrv_Init(void)
{
    /* 初始化 NVM 参数模块 */
    if (nvm_param_init() != 0) {
        return false;
    }
    
    /* 加载当前参数到内存缓存 */
    return ParamSrv_Load(&g_current_param);
}

/**
 * @brief  读取当前参数
 * @param  param: 输出参数指针
 * @retval true: 成功, false: 失败
 * @note   直接返回内存缓存，无需NVM交互
 */
bool ParamSrv_Load(param_config_t *param)
{
    if (param == NULL) {
        return false;
    }
    
    /* 直接从内存缓存复制 */
    *param = g_current_param;
    return true;
}

/**
 * @brief  保存参数到 Flash
 * @param  param: 要保存的参数
 * @retval true: 成功, false: 失败
 */
bool ParamSrv_Save(const param_config_t *param)
{
    if (param == NULL) {
        return false;
    }
    
    /* 保存到 Flash */
    if (nvm_param_set(param) != 0) {
        return false;
    }
    
    /* 更新内存缓存 */
    g_current_param = *param;
    return true;
}

/**
 * @brief  将当前参数应用到 ISL700 芯片
 * @retval true: 成功, false: 失败
 * @note   只传递 ISL700 配置参数，不传递应用层参数
 *         应用层参数不影响 IC 配置
 */
bool ParamSrv_ApplyToIC(void)
{
    /* 只传递 ISL700 配置参数部分，不使用应用层参数 */
    return ISL700_Write_Reg(&g_current_param.isl700);
}

/**
 * @brief  恢复出厂默认值
 * @retval true: 成功, false: 失败
 * @note   恢复默认值并保存到 Flash，但不自动应用到芯片
 */
bool ParamSrv_RestoreDefault(void)
{
    /* 恢复出厂默认值 */
    if (nvm_param_restore_default() != 0) {
        return false;
    }
    /* 重新加载参数 */
    return ParamSrv_Load(&g_current_param);
}

/* ==================== 测量服务 API 实现 ==================== */

void MeasSrv_Start(void)
{
    
}

void MeasSrv_Stop(void)
{
    
}

/* sysTick interrupt call back -------------------------------------------------------------- */
void SysTickCallBack (void)
{
    BtnTask();
	LedTask();
}
