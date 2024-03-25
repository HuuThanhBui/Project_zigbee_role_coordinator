#ifndef _LIB_I2C_H_
#define _LIB_I2C_H_

#include "stdint.h"

void initCMU(void);
void initI2C(void);
void I2C_Disable(void);
void I2C_Read(uint16_t SensorAddress, uint8_t targetAddress, uint8_t *rxBuff, uint8_t numBytes);
void I2C_Write(uint8_t address, uint8_t reg, uint8_t *value, uint8_t numBytes);

#endif
