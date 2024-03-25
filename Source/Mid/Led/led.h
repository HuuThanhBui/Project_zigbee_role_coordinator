#ifndef SOURCE_MID_LED_LED_H_
#define SOURCE_MID_LED_LED_H_
#include "em_cmu.h"
#include "em_gpio.h"

typedef enum{
	porta,
	portb,
	portc,
	portd
}gpio_port_t;

void ledinit(gpio_port_t gpio_port, unsigned int pin);
void ledoff(gpio_port_t gpio_port, unsigned int pin);
void ledon(gpio_port_t gpio_port, unsigned int pin);
void ledtoggle(gpio_port_t gpio_port, unsigned int pin);
void leduninit(void);

#endif
