#include "stdint.h"
#include "stdbool.h"
#include "Source/Mid/LM75/LM75.h"
#include "Source/Driver/i2c.h"
#include "app/framework/include/af.h"
#include "Source/Mid/Kalman_Fillter/filter.h"

#define USE_KALMAN_FILLTER		//option: If we want use Kalman fillter to fill data of sensor, uncomment this option

/*
	func: LM75_Init
	brief: Initiation clock of peripheral GPIO, I2C0 and init some param of I2C use to communication with LM75 sensor
	param: NONE
	retval: NONE
 */

void LM75_Init(void)
{
	initCMU();
	initI2C();
#ifdef USE_KALMAN_FILLTER
	KalmanFilterInit(2,2,0.01);
#endif
}

/*
	func: LM75_Disable
	brief: Disable peripheral I2C0
	param: NONE
	retval: NONE
 */

void LM75_Disable(void)
{
	I2C_Disable();
}

/*
	func: LM75_read_temp
	brief: Read data value of sensor LM75 use I2C0 to communication LM75 sensor
	param: NONE
	retval: Value data tempeture of sensor LM75
 */

double LM75_read_temp(void)
{
	uint8_t raw_data[2] = {0};
	uint16_t data = 0;
	uint32_t count = 0;
	double temp_value = 0;
	I2C_Write(0x90,0x00,raw_data,2);
	while(halCommonGetInt32uMillisecondTick() - count <=200)
	{
		;
	}
	I2C_Read(0x90,0x00,raw_data,2);
	data = (uint16_t)(raw_data[0] << 8) | raw_data[1];
#ifdef USE_KALMAN_FILLTER
	data = KalmanFilter_updateEstimate((float)data);
#endif
	temp_value = (double)data/256.0;
	return temp_value;
}
