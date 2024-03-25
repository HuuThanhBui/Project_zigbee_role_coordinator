#include "app/framework/include/af.h"
#include "em_timer.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "led.h"

void ledinit(gpio_port_t gpio_port, unsigned int pin)
{
	GPIO_Port_TypeDef port;
	GPIO_Mode_TypeDef mode;
	CMU_ClockEnable(cmuClock_GPIO,true);

	if(gpio_port == porta){port = gpioPortA;}
	else if(gpio_port == portb){port = gpioPortB;}
	else if(gpio_port == portc){port = gpioPortC;}
	else if(gpio_port == portd){port = gpioPortD;}

	mode = gpioModePushPull;

	GPIO_PinModeSet(port,pin,mode,0);
}

void leduninit(void)
{
	CMU_ClockEnable(cmuClock_GPIO,false);
}

void ledoff(gpio_port_t gpio_port, unsigned int pin)
{
	GPIO_Port_TypeDef port;
	if(gpio_port == porta){port = gpioPortA;}
	else if(gpio_port == portb){port = gpioPortB;}
	else if(gpio_port == portc){port = gpioPortC;}
	else if(gpio_port == portd){port = gpioPortD;}
	GPIO_PinOutSet(port,pin);
}

void ledon(gpio_port_t gpio_port, unsigned int pin)
{
	GPIO_Port_TypeDef port;
	if(gpio_port == porta){port = gpioPortA;}
	else if(gpio_port == portb){port = gpioPortB;}
	else if(gpio_port == portc){port = gpioPortC;}
	else if(gpio_port == portd){port = gpioPortD;}
	GPIO_PinOutClear(port,pin);
}

void ledtoggle(gpio_port_t gpio_port, unsigned int pin)
{
	GPIO_Port_TypeDef port;
	if(gpio_port == porta){port = gpioPortA;}
	else if(gpio_port == portb){port = gpioPortB;}
	else if(gpio_port == portc){port = gpioPortC;}
	else if(gpio_port == portd){port = gpioPortD;}
	GPIO_PinOutToggle(port, pin);
}








