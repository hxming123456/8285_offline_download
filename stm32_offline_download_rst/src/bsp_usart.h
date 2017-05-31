#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stm32f10x.h"
#include "bsp_time_delay.h"
#include "Data_pool.h"

#define DATA_SEND_RECV_DABUG    'N'
#define INFO_DEBUG   			'Y'

extern uint32_t usart_send_timeout;

void usart1_init();
void usart1_write(uint8_t *data,uint32_t data_len);
int usart1_read(uint8_t *data);
void usart1_gpio_init(void);
void usart1_change_baud(uint32_t baud);
void usart1_nvic_init(void);

void Debug_usart_init();
void Debug_usart_gpio_init(void);
void Debug_usart_write(void *data,uint32_t data_len,uint8_t debug_type);
int Debug_usart_read(uint8_t *data);
void Debug_usart_nvic_init(void);

#endif
