#include "uart.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include "efr32mg21_usart.h"
#include "app/framework/include/af.h"
#include "zigbee-framework/zigbee-device-common.h"
#include "app/framework/util/af-main.h"
#include "bsp.h"

function_callback_usart_recive callback_data_usart;

/**************************************************************************//**
 * @brief
 *    CMU initialization
 *****************************************************************************/
static void initCMU(void)
{
  // Enable clock to GPIO and USART1
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART1, true);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
static void initGPIO(gpio_port_t gpio_port_tx, unsigned int pin_tx, gpio_port_t gpio_port_rx, unsigned int pin_rx)
{
	GPIO_Port_TypeDef port_tx = gpioPortA, port_rx = gpioPortA;
	CMU_ClockEnable(cmuClock_GPIO,true);

	if(gpio_port_tx == porta){port_tx = gpioPortA;}
	else if(gpio_port_tx == portb){port_tx = gpioPortB;}
	else if(gpio_port_tx == portc){port_tx = gpioPortC;}
	else if(gpio_port_tx == portd){port_tx = gpioPortD;}


	if(gpio_port_rx == porta){port_rx = gpioPortA;}
	else if(gpio_port_rx == portb){port_rx = gpioPortB;}
	else if(gpio_port_rx == portc){port_rx = gpioPortC;}
	else if(gpio_port_rx == portd){port_rx = gpioPortD;}
	// Configure the USART TX pin to the board controller as an output
	GPIO_PinModeSet(port_tx, pin_tx, gpioModePushPull, 0);

	// Configure the USART RX pin to the board controller as an input
	GPIO_PinModeSet(port_rx, pin_rx, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief
 *    The USART1 receive interrupt saves incoming characters.
 *****************************************************************************/
void USART1_RX_IRQHandler(void)
{
	static char arr_data[50] = {0};
	char arr_data_callback[50] = {0};
	static uint32_t count = 0;
	if(USART1->RXDATA != '\0' || USART1->RXDATA != '\n')
	{
		arr_data[count++] = (char)USART1->RXDATA;
//		emberAfCorePrint("%c",USART1->RXDATA);
	}
	if(USART1->RXDATA == '\0' || USART1->RXDATA == '\n')
	{
		if(count < 20)
		{
			count--;
			strncpy(arr_data_callback,arr_data,count);
			callback_data_usart(arr_data_callback, count);
		}
		count = 0;
	}
}

/**************************************************************************//**
 * @brief
 *    USART1 initialization
 *****************************************************************************/
void initUSART1(gpio_port_t gpio_port_tx, unsigned int pin_tx, gpio_port_t gpio_port_rx, unsigned int pin_rx, uint32_t baudrate, function_callback_usart_recive cb_data_rx)
{
	// Default asynchronous initializer (115.2 Kbps, 8N1, no flow control)
	GPIO_Port_TypeDef port_tx = gpioPortA, port_rx = gpioPortA;
	USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
	init.baudrate = baudrate;

	if(cb_data_rx != NULL)
	{
		callback_data_usart = cb_data_rx;
	}

	CHIP_Init();
	initCMU();
	initGPIO(gpio_port_tx, pin_tx, gpio_port_rx, pin_rx);

	if(gpio_port_tx == porta){port_tx = gpioPortA;}
	else if(gpio_port_tx == portb){port_tx = gpioPortB;}
	else if(gpio_port_tx == portc){port_tx = gpioPortC;}
	else if(gpio_port_tx == portd){port_tx = gpioPortD;}


	if(gpio_port_rx == porta){port_rx = gpioPortA;}
	else if(gpio_port_rx == portb){port_rx = gpioPortB;}
	else if(gpio_port_rx == portc){port_rx = gpioPortC;}
	else if(gpio_port_rx == portd){port_rx = gpioPortD;}
	// Route USART1 TX and RX to the board controller TX and RX pins
	GPIO->USARTROUTE[1].TXROUTE = (port_tx << _GPIO_USART_TXROUTE_PORT_SHIFT)
	  | (pin_tx << _GPIO_USART_TXROUTE_PIN_SHIFT);
	GPIO->USARTROUTE[1].RXROUTE = (port_rx << _GPIO_USART_RXROUTE_PORT_SHIFT)
	  | (pin_rx << _GPIO_USART_RXROUTE_PIN_SHIFT);

	// Enable RX and TX signals now that they have been routed
	GPIO->USARTROUTE[1].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN;

	// Configure and enable USART1
	USART_InitAsync(USART1, &init);

	// Enable NVIC USART sources
	NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(USART1_RX_IRQn);

	USART_IntEnable(USART1, USART_IEN_RXDATAV);		//Bat ngat RX
}

void USART1_send(USART_TypeDef *usart, char data[], uint32_t leng)
{
	for(uint32_t i = 0; i < leng; i++)
	{
		USART_Tx(usart, (uint8_t)data[i]);
	}
}

void USART1_interrupt_enable(void)
{
	USART_IntEnable(USART1, USART_IEN_RXDATAV);
}

void USART1_interrupt_disable(void)
{
	USART_IntDisable(USART1, USART_IEN_RXDATAV);
}





