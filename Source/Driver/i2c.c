#include "Source/Driver/i2c.h"
#include "app/framework/include/af.h"
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"

/*
	func: initCMU
	brief: Initiation clock of peripheral GPIO and I2C0
	param: NONE
	retval: NONE
 */

void initCMU(void)
{
	CMU_ClockEnable(cmuClock_I2C0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
}

/*
	func: initI2C
	brief: Initiation some param of peripheral I2C0 (such as: Type of I2C, GPIO to use I2C, ...)
	param: NONE
	retval: NONE
 */

void initI2C(void)
{
//	static uint8_t status = 0;
//	if(status == 1)
//	{
//		return;
//	}
	I2C_Init_TypeDef init_I2C =
	{
			.enable = true,
			.clhr = i2cClockHLRStandard,
			.freq = I2C_FREQ_STANDARD_MAX,
			.master = true,
			.refFreq = 0
	};

	CMU_ClockEnable(cmuClock_HCLK, true);

	CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_LSPCLK,cmuSelect_LFRCO);

	// Using PA4 (SDA) and PC2 (SCL)
	GPIO_PinModeSet(gpioPortA, 4, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(gpioPortC, 2, gpioModeWiredAndPullUpFilter, 1);

	// Route I2C pins to GPIO
	GPIO->I2CROUTE[0].SDAROUTE = (GPIO->I2CROUTE[0].SDAROUTE & ~_GPIO_I2C_SDAROUTE_MASK)
						| (gpioPortA << _GPIO_I2C_SDAROUTE_PORT_SHIFT
						| (4 << _GPIO_I2C_SDAROUTE_PIN_SHIFT));
	GPIO->I2CROUTE[0].SCLROUTE = (GPIO->I2CROUTE[0].SCLROUTE & ~_GPIO_I2C_SCLROUTE_MASK)
						| (gpioPortC << _GPIO_I2C_SCLROUTE_PORT_SHIFT
						| (2 << _GPIO_I2C_SCLROUTE_PIN_SHIFT));
	GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;

	// Initialize the I2C
	I2C_Init(I2C0, &init_I2C);
	// Enable automatic STOP on NACK
	I2C0->CTRL = I2C_CTRL_AUTOSN;
	I2C_Enable(I2C0, true);
//	status = 1;
}

/*
	func: I2C_Read
	brief: Read byte data on bus I2C of numBytes need to read.
	param:
		SensorAddress: Address of slave I2C device on bus I2C need to comunication.
		targetAddress: Address register of slave I2C device on bus I2C need to comunication.
		*rxBuff: Buffer to contain number of numBytes data.
		numBytes: Number of data need to read.
	retval: NONE
 */

void I2C_Read(uint16_t SensorAddress, uint8_t targetAddress, uint8_t *rxBuff, uint8_t numBytes)
{
	// Transfer structure
	I2C_TransferSeq_TypeDef i2cTransfer;
	I2C_TransferReturn_TypeDef result;


	// Initialize I2C transfer
	i2cTransfer.addr          = SensorAddress;
	i2cTransfer.flags         = I2C_FLAG_WRITE_READ; // must write target address before reading
	i2cTransfer.buf[0].data   = &targetAddress;
	i2cTransfer.buf[0].len    = 1;
	i2cTransfer.buf[1].data   = rxBuff;
	i2cTransfer.buf[1].len    = numBytes;

	result = I2C_TransferInit(I2C0, &i2cTransfer);

	// Send data
	while (result == i2cTransferInProgress) {
		result = I2C_Transfer(I2C0);
	}
}

/*
	func: I2C_Write
	brief: Write numBytes byte data on bus I2C.
	param:
		address: Address of slave I2C device on bus I2C need to comunication.
		reg: Address register of slave I2C device on bus I2C need to comunication.
		*value: Buffer to contain number of numBytes data need to write on bus I2C.
		numBytes: Number of data need to write in bus I2C.
	retval: NONE
 */

void I2C_Write(uint8_t address, uint8_t reg, uint8_t *value, uint8_t numBytes)
{
  // Transfer structure
  I2C_TransferSeq_TypeDef i2cTransfer;
  I2C_TransferReturn_TypeDef result;

  // Initializing I2C transfer
  i2cTransfer.addr          = address;
  i2cTransfer.flags         = I2C_FLAG_WRITE_WRITE;
  i2cTransfer.buf[0].data   = &reg;
  i2cTransfer.buf[0].len    = 1;
  i2cTransfer.buf[1].data   = value;
  i2cTransfer.buf[1].len    = numBytes;

  result = I2C_TransferInit(I2C0, &i2cTransfer);

  // Sending data
  while (result == i2cTransferInProgress)
  {
    result = I2C_Transfer(I2C0);
  }
}

/*
	func: I2C_Disable
	brief: Reset bus I2C0
	param: NONE
	retval: NONE
 */

void I2C_Disable(void)
{
	I2C_Reset(I2C0);
	I2C_Enable(I2C0, false);
}

