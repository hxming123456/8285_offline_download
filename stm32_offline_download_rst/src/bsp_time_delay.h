#ifndef __BSP_TIME_DELAY_H
#define __BSP_TIME_DELAY_H

#include "stm32f10x.h"
#include "Data_pool.h"
#include "8285_download_protocol.h"

extern uint32_t recv_timeout_start;
extern uint32_t recv_timeout_end;
extern uint32_t recv_timeout;
extern uint32_t pool_wait_time;

void tim_init(void);
void delay_ms(int32_t timecnt);
void delay_s(int32_t scnt);
void change_light_status(uint8_t status);

#endif
