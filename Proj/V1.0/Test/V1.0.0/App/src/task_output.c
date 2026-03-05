#include "main.h"

__IO output_state_t g_outputState = NO_OBJECT;
//if (V_OUT >= ParamCfg_GetVset())

void OutputTask(void)
{
	/* 1. Judge Presence */
	
	
	if (V_OUT >= 100) {
		g_outputState = OBJECT_PRESENT;
		//printf("V_OUT : %d\r\n",V_OUT);
        printf("Object Present!\r\n");
    }else if(V_OUT < 100){
		g_outputState = NO_OBJECT;
        printf("No Object.\r\n");
    }else if(V_OUT< 0){
		printf("Invalied Object.\r\n");
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
