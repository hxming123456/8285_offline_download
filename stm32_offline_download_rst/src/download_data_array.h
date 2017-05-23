#ifndef __DOWNLOAD_DATA_ARRAY_H
#define	__DOWNLOAD_DATA_ARRAY_H

#include "stm32f10x.h"

extern uint8_t  sign_sector_data[4095];
extern uint8_t  data_sector_data[4095];
extern uint8_t  stub_text_up[6168];
extern uint8_t  stub_text_down[1528];
extern uint8_t  stub_data[792];

#endif
