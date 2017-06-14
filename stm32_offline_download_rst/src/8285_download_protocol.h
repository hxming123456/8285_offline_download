#ifndef __8285_DOWNLOAD_PROTOCOL_H
#define __8285_DOWNLOAD_PROTOCOL_H

#include "stm32f10x.h"
#include "clib.h"
#include "bsp_usart.h"
#include "Data_pool.h"
#include ".//ff9//ff.h"
#include ".//ff9//bsp_sdio_sdcard.h"
#include "download_data_array.h"
#include "bsp_iwdg.h"
#include <string.h>

extern uint8_t nodata_flag;
extern uint32_t recv_time_out;
extern int32_t delay_time;
extern uint8_t sync_flag;
extern uint8_t wait_baud_flag;
extern uint8_t update_status;
extern uint8_t all_time_flag;
extern uint32_t send_time;
extern uint8_t recv_over_flag;

#define FIRMWARE_BIN 			0x00000000
#define DATA_INFO				0x00078000
#define SIGN_INFO				0x0007a000

#define MEM_BEGIN_TEXT			0x02
#define MEM_BEGIN_DATA			0x01

#define STUB_TEXT_UP			0x03
#define STUB_TEXT_DOWN			0x04
#define STUB_DATA				0x05

#define NO_HEAD_NO_TAIL			0x06
#define NO_HEAD_HA_TAIL			0x07
#define HA_HEAD_NO_TAIL			0x08
#define HA_HEAD_HA_TAIL			0x09

#define DOWNLOAD_OK_STATUS      0x0a
#define DOWNLOAD_NOK_STATUS     0x0b
#define DOWNLOAD_NOW_STATUS     0x0c
#define NO_DATA_STATUS			0x0e

#define STUB_TEXT_UP_LEN        6168
#define STUB_TEXT_DOWN_LEN		1528
#define STUB_DATA_LEN           792

#define DOWNLOAD_OK				1
#define DOWNLOAD_NOK			2
#define NO_DATA					3

#define SDCRAD_LOG				'Y'


void Data_formatt_write(uint8_t *packet,int packet_len,uint8_t packet_type);
int Data_formatt_read(uint8_t *packet,int len);
int device_sync(void);
int Change_baud_command(int baud);
int Erasing_data_command(int file_type);
int stub_mem_finish(void);
int stub_mem_begin(uint8_t type);
int stub_mem_block(uint8_t type);
int run_stub(void);
uint8_t checksum(int type,int data_len);
int send_data_command(int type,int data_len,uint8_t seq);
int Data_formatt_write_modify(int packet_len,uint8_t packet_type,uint8_t cnt);
int download_start(int baud,uint8_t isdata_flag);
void wait_rxdata_available(int timeout);
void change_datacsv_info(uint8_t *buf);
void change_sign_info(void);
void wait_baud_change(int timeout);
int rst_8266(void);
int SDcard_log_write(uint8_t *buf,int32_t len,uint8_t islog_flag);
int check_file_from_filedir(uint8_t isflag);

#endif
