#include "stdint.h"
#include "stdbool.h"
#include "Source/Mid/BH1750/bh1750.h"
#include "Source/Driver/i2c.h"
#include "app/framework/include/af.h"
#include "Source/Mid/Kalman_Fillter/filter.h"

#define USE_KALMAN_FILLTER		//option: If we want use Kalman fillter to fill data of sensor, uncomment this option

/*
	func: BH1750_Init
	brief: Initiation clock of peripheral GPIO, I2C0, and some param of peripheral I2C0 to use sensor BH1750
	param: NONE
	retval: NONE
 */

void BH1750_Init(void)
{
	initCMU();
	initI2C();
#ifdef USE_KALMAN_FILLTER
	KalmanFilterInit(2,2,0.01);
#endif
}

/*
	func: BH1750_Disable
	brief: Disable I2C peripheral.
	param: NONE
	retval: NONE
 */

void BH1750_Disable(void)
{
	I2C_Disable();
}

/*
	func: BH1750_read_light
	brief: Use I2C protocol to read data of sensor BH1750
	param: NONE
	retval: Light value of sensor BH1750
 */

double BH1750_read_light(void)
{
	uint8_t raw_data[2] = {0};
	uint16_t data = 0;
	uint32_t count = 0;
	double light_value = 0;
	I2C_Write(0x46,0x10,raw_data,2);
	while(halCommonGetInt32uMillisecondTick() - count <=200)
	{
		;
	}
	I2C_Read(0x46,0x10,raw_data,2);
	data = (uint16_t)(raw_data[0] << 8) | raw_data[1];
#ifdef USE_KALMAN_FILLTER
	data = KalmanFilter_updateEstimate((float)data);
#endif
	light_value = (double)data/1.2;
	return light_value;
}
