#ifndef __BSP_TIME_DELAY_H
#define __BSP_TIME_DELAY_H

#include "stm32f10x.h"
#include "Data_pool.h"
#include "8285_download_protocol.h"

void tim_init(void);
void delay_ms(int32_t timecnt);
void delay_s(int32_t scnt);
void change_light_status(uint8_t status);

#endif
