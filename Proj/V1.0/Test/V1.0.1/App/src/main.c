#include "main.h"
#include "SEGGER_RTT.h"

#define APP_VERSION		"1.0.1"

__IO bool g_isSampleDone = true;
__IO bool g_isStartSampling = false;
__IO bool g_isSysTickInt = false;

void ParamCfg_Print(volatile param_config_t *cfg);


volatile param_config_t g_param_cfg = PARAM_CONFIG_DEFAULT; 

int main(void)
{
    printf("App Version: %s\r\n", APP_VERSION);
	BSP_Init();
    Task_Init();
    ISL700_Init(&g_param_cfg.isl700);
	delay_1ms(500);
    while (1) {
		start_timing_us();
        if (g_isSysTickInt) {
			g_isSysTickInt = false;
			//SysTickTask();
		}

        if(g_isStartSampling) {
            MeasureTask();
			g_isStartSampling = false;
        } 

        if(g_isSampleDone) {
			
            AnalyzeTask();  
            OutputTask();
            LedTask();
            g_isSampleDone = false;
        }
		print_timing_us(stop_timing_us());
    }
}

void ParamCfg_Print(volatile param_config_t *cfg)
{
    if (cfg == NULL) {
        return;
    }

    printf("=====================\r\n");    
    printf("=== IC Registers ===\r\n");
    printf(" FUSE_F:         %u\r\n", (unsigned)cfg->isl700.fuse_check_floating);
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
    

    printf("==== APP Params ====\r\n");
    printf(" V_SET:          %u\r\n", (unsigned)cfg->app.v_set);
    printf(" LED:            %u\r\n", (unsigned)cfg->app.led_mode);

    printf("=====================\r\n");
}

