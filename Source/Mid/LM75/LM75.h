#ifndef _LIB_SENSOR_LM75_H_
#define _LIB_SENSOR_LM75_H_

#include "stdint.h"
#include "stdbool.h"

void LM75_Init(void);
void LM75_Disable(void);
double LM75_read_temp(void);

#endif
