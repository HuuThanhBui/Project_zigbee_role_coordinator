#ifndef _LIB_SENSOR_BH1750_H_
#define _LIB_SENSOR_BH1750_H_

#include "stdint.h"
#include "stdbool.h"

void BH1750_Init(void);
void BH1750_Disable(void);
double BH1750_read_light(void);

#endif
