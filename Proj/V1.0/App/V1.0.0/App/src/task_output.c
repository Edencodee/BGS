#include "main.h"

__IO output_state_t g_outputState = NO_OBJECT;


void OutputTask(void)
{
	/* 1. Judge Presence */
	
	if (V_OUT >= ParamCfg_GetVset()) {
		g_outputState = OBJECT_PRESENT;
        printf("Object Present!\r\n");
    }else{
		g_outputState = NO_OBJECT;
        printf("No Object.\r\n");
    }

	/* 2. Short Circuit Protection */
    if (g_outputState == SHORT_CIRCUIT) {
        NPN_Clr();  
        return;
    }
	/* 3. Get Output State */
	if (g_outputState == OBJECT_PRESENT) {
		NPN_Set();
	} else {
		NPN_Clr();
	}
}
