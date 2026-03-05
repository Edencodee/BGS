#include "main.h"
#include "SEGGER_RTT.h"

#define APP_VERSION		"1.0.0"

__IO bool g_isSampleDone = true;
__IO bool g_isStartSampling = false;
__IO bool g_isSysTickInt = false;
param_config_t g_current_cfg = PARAM_CONFIG_DEFAULT; 

          
static void ParamCfg_Demo(void);
static void ParamCfg_Print(const param_config_t *cfg);

int main(void)
{
    printf("App Version: %s\r\n", APP_VERSION);
	BSP_Init();
    Task_Init();
	ParamCfg_ApplyToIC();
    BSP_ICPinConfig();
    delay_1ms(2000); 
    //ParamCfg_Demo();
    ParamCfg_Get(&g_current_cfg);
	printf("g_current_cfg\r\n");
    ParamCfg_Print(&g_current_cfg); 

	delay_1ms(2000); 
    while (1) {
		exti_software_interrupt_enable(PLS_SO_EXTI_LINE);
		delay_1ms(50);
        if (g_isSysTickInt) {
			g_isSysTickInt = false;
			SysTickTask();
		}

        if(g_isStartSampling) {
            g_isStartSampling = false;
            MeasureTask();
        } 

        if(g_isSampleDone) {
			g_isSampleDone = false;
            AnalyzeTask();  
            OutputTask();
            LedTask();
        }   
    }
}

static void ParamCfg_Demo(void)
{
    param_config_t cfg;
    
    /* 读取当前配置（来自缓存） */
    if (ParamCfg_Get(&cfg)) {
        ParamCfg_Print(&cfg);
    }

    /* 修改参数并应用到 IC，同时保存到 Flash */
    cfg.isl700.pls = 200;
    cfg.isl700.fine_gain = 80;
    cfg.app.v_set = 1000;
    cfg.app.led_mode = 1;
    ParamCfg_Set(&cfg);
    ParamCfg_ApplyToIC();
    ParamCfg_Save(&cfg);

    /* 从 Flash 重新加载并读取验证 */
    ParamCfg_Init();
    if (ParamCfg_Get(&cfg)) {
        ParamCfg_Print(&cfg);
    }

    /* 再次修改部分参数、保存并重新读取 */
    cfg.isl700.pls = 180;
    cfg.app.v_set = 1200;
    cfg.app.led_mode = 0;
    ParamCfg_Set(&cfg);
    ParamCfg_Save(&cfg);
    ParamCfg_Init();
    if (ParamCfg_Get(&cfg)) {
        ParamCfg_Print(&cfg);
    }
}

static void ParamCfg_Print(const param_config_t *cfg)
{
    if (cfg == NULL) {
        return;
    }
    printf("=====================\r\n");    
    printf("=== IC Registers ===\r\n");
    printf(" FUSE_L:      %u\r\n", (unsigned)cfg->isl700.fuse_check_low);
    printf(" PLS:         %u\r\n", (unsigned)cfg->isl700.pls);
    printf(" NO_USED:     %u\r\n", (unsigned)cfg->isl700.no_used);
    printf(" FINE_GAIN:   %u\r\n", (unsigned)cfg->isl700.fine_gain);
    printf(" GAIN_SEL:    %u\r\n", (unsigned)cfg->isl700.gain_select);
    printf(" FUSE_M:      %u\r\n", (unsigned)cfg->isl700.fuse_check_middle);
    printf(" CG1:         %u\r\n", (unsigned)cfg->isl700.coarse_gain1);
    printf(" CG2:         %u\r\n", (unsigned)cfg->isl700.coarse_gain2);
    printf(" SRA:         %u\r\n", (unsigned)cfg->isl700.sctrl_res_a);
    printf(" SRB:         %u\r\n", (unsigned)cfg->isl700.sctrl_res_b);
    printf(" SCTRL_INT:   %u\r\n", (unsigned)cfg->isl700.sctrl_internal);
    printf(" FUSE_H:      %u\r\n", (unsigned)cfg->isl700.fuse_check_high);
    printf(" FUSE_F:      %u\r\n", (unsigned)cfg->isl700.fuse_check_floating);

    printf("=== APP Params ===\r\n");
    printf(" V_SET:       %u\r\n", (unsigned)cfg->app.v_set);
    printf(" LED:         %u\r\n", (unsigned)cfg->app.led_mode);

    printf("=====================\r\n");
}

