#ifndef __BSP_EEPROM_H
#define __BSP_EEPROM_H

#include "stm32f10x.h"
#include "bsp_iic_gpio_simulate.h"
#include "bsp_usart.h"

uint8_t eep_write_data(uint8_t *data,uint8_t count,uint8_t addr);
uint8_t eep_read_data(uint8_t *data,uint8_t count,uint8_t addr);

#endif
