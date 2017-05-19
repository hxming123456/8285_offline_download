#ifndef __BSP_TIME_DELAY_H
#define __BSP_TIME_DELAY_H

#include "stm32f10x.h"
#include "Data_pool.h"

extern int32_t recv_time_out;
extern int32_t cnt_time;

void delay_init(void);
void delay_ms(uint32_t timecnt);
void delay_s(uint32_t scnt);

#endif
