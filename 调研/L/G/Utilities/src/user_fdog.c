#include "user_fdog.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "wtdint.h"

void fdog_init(void)
{
   /* confiure FWDGT counter clock: 32KHz(IRC32K) / 64 = 0.5 KHz */
    fwdgt_config(3 * 500, FWDGT_PSC_DIV64);
    /* after 3 seconds to generate a reset */
    fwdgt_enable();
}

void feed_dog(void)
{
		fwdgt_counter_reload();
}