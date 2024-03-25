#ifndef _UART_H_
#define _UART_H_

#include "Source/Mid/Led/led.h"
#include "stdint.h"

typedef void(*function_callback_usart_recive)(char data[], uint32_t leng);

void initUSART1(gpio_port_t gpio_port_tx, unsigned int pin_tx, gpio_port_t gpio_port_rx, unsigned int pin_rx, uint32_t baudrate, function_callback_usart_recive cb_data_rx);
void USART1_interrupt_enable(void);
void USART1_interrupt_disable(void);
void USART1_send(USART_TypeDef *usart, char data[], uint32_t leng);

#endif



