#ifndef __USER_FDOG_H
#define __USER_FDOG_H

#ifdef cplusplus
extern "C" {
#endif

#include "gd32c2x1.h"
#include <stdbool.h>

	void fdog_init(void);
	void feed_dog(void);
#ifdef cplusplus
}
#endif

#endif
