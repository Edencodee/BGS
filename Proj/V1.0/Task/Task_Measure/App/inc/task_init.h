#ifndef __TASK_INIT_H
#define __TASK_INIT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "gd32c2x1.h"
#include "stdbool.h"
#include "param_config.h"


/* function declaration ------------------------------- */
    void Task_Init(void);
    
    /* ===== 参数服务 API ===== */
    bool ParamSrv_Init(void);                                   // 初始化参数服务（从 Flash 加载）
    bool ParamSrv_Load(param_config_t *param);                  // 读取当前参数
    bool ParamSrv_Save(const param_config_t *param);            // 保存参数到 Flash
    bool ParamSrv_ApplyToIC(void);                              // 将当前参数应用到 ISL700 芯片
    bool ParamSrv_RestoreDefault(void);                         // 恢复出厂默认值
    
    /* ===== 测量服务 API ===== */
	void SysTickCallBack (void);
    void MeasSrv_Start(void);
    void MeasSrv_Stop(void);
/* ---------------------------------------------------- */



#ifdef __cplusplus
}
#endif

#endif
