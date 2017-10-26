//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f10x.h"
#include "bsp_time_delay.h"
#include "bsp_usart.h"
#include "clib.h"
#include "Data_pool.h"
#include ".//ff9//ff.h"
#include ".//ff9//bsp_sdio_sdcard.h"
#include "bsp_iwdg.h"
#include "8285_download_protocol.h"
#include "bsp_eeprom.h"



// ----------------------------------------------------------------------------
//
// Standalone STM32F1 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

ErrorStatus HSEStartUpStatus;

uint32_t use_cnt = 0;

extern int32_t cnt_time;

#define ISDATA_PIN_READ GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)
#define EEP_ADDRESS	0x01


void system_clk_init(void)
{
	RCC_DeInit();

	RCC_HSEConfig(RCC_HSE_ON);

	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus)
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
	}

	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
	RCC_PLLCmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
}

void light_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//ok nok now
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
}

void is_download_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void rst_pin_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB,GPIO_Pin_9);
}

void isdata_pin_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void SD_test()
{
	//uint8_t buf[98] = {0};
	//uint32_t num;
	//FIL fnew;

#if 0
	res = f_open(&fnew, "0:data.csv",FA_READ|FA_WRITE);

	if ( res == FR_OK )
	{
		Debug_usart_write("F_OPEN OK\r\n",11,INFO_DEBUG);
	}
	else if ( res == FR_EXIST )
	{
		Debug_usart_write("F_EXIST\r\n",9,INFO_DEBUG);
	}
	else
	{
		Debug_usart_write(&res,1);
	}

	res = f_read(&fnew,buf,98,&num);
	if ( res == FR_OK )
	{
		Debug_usart_write("F_READ OK\r\n",11,INFO_DEBUG);
	}
	else
	{
		Debug_usart_write(&res,1);
	}

	usart2_write(buf,num);
	buf[0] = 'N';
	res = f_lseek(&fnew,0*98);
	if(res == FR_OK)
	{
		Debug_usart_write("change file point ok\r\n",22,INFO_DEBUG);
	}

	res = f_write(&fnew,buf,98,&num);
	if ( res == FR_OK )
	{
		Debug_usart_write("F_WRITE OK\r\n",11,INFO_DEBUG);
	}
	else
	{
		Debug_usart_write(&res,1);
	}
#if 0
	res = f_read(&fnew,buf,46,&num);
	if ( res == FR_OK )
	{
		//Debug_usart_write("F_READ OK\r\n",11,INFO_DEBUG);
	}
	else
	{
		Debug_usart_write(&res,1);
	}

	Debug_usart_write(&num,1);
#endif

	f_close(&fnew);
#endif
}

void Get_eeprom_data(void)
{
	uint8_t buf[5] = {0x00};
	uint8_t num_str[10] = {0x00};
	uint8_t ret = 0;

#if 1
	ret = eep_read_data(buf,5,EEP_ADDRESS);
	if(ret != 0)
	{
		Debug_usart_write("read EEPROM NOK\r\n",17,INFO_DEBUG);
	}

	if(buf[0] == 'Y')
	{
		use_cnt = (((((buf[1]>>8)+buf[2])<<8)+buf[3])<<8)+buf[4];//00000000 00000000 00000000 00000001  1(uint32_t)
		Itoa(use_cnt,num_str);									 //buf[1]	buf[2]	 buf[3]	  buf[4]
		Debug_usart_write("eep read:",9,INFO_DEBUG);
		Debug_usart_write(num_str,10,INFO_DEBUG);
		Debug_usart_write("\r\n",2,INFO_DEBUG);
		Debug_usart_write("read EEPROM OK\r\n",16,INFO_DEBUG);
	}
	else
#endif
	{
		buf[0] = 'Y';
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;
		buf[4] = 0;
		ret = eep_write_data(buf,5,EEP_ADDRESS);
		if(ret != 0)
		{
			Debug_usart_write("write EEPROM NOK\r\n",18,INFO_DEBUG);
		}
		else
		{
			Debug_usart_write("write EEPROM OK\r\n",17,INFO_DEBUG);
		}
	}
	iwdg_reload();
}

void update_use_cnt_to_eeprom(uint32_t use_cnt)
{
	uint8_t ret = 0;
	uint8_t buf[5] = {0};
	uint32_t test = 0;
	uint8_t num_str[6] = {0};

	buf[0] = 'Y';
	buf[1] = (use_cnt>>24)&0xff;
	buf[2] = (use_cnt>>16)&0xff;
	buf[3] = (use_cnt>>8)&0xff;
	buf[4] = use_cnt&0xff;
	ret = eep_write_data(buf,5,EEP_ADDRESS);
	if(ret != 0)
	{
		Debug_usart_write("update EEPROM NOK\r\n",19,INFO_DEBUG);
	}
	else
	{
		Debug_usart_write("update EEPROM OK\r\n",18,INFO_DEBUG);
	}

	Debug_usart_write("update:",7,INFO_DEBUG);
	Itoa(use_cnt,num_str);
	Debug_usart_write(num_str,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	ret = eep_read_data(buf,5,EEP_ADDRESS);
	if(ret != 0)
	{
		Debug_usart_write("check update eep NOK\r\n",22,INFO_DEBUG);
	}
	else
	{
		test = (((((buf[1]>>8)+buf[2])<<8)+buf[3])<<8)+buf[4];

		Debug_usart_write("check:",6,INFO_DEBUG);
		memset(num_str,0,5);
		Itoa(test,num_str);
		Debug_usart_write(num_str,5,INFO_DEBUG);
		Debug_usart_write("\r\n",2,INFO_DEBUG);

		if(test == use_cnt)
		{
			Debug_usart_write("check update eep OK\r\n",21,INFO_DEBUG);
		}
		else
		{
			Debug_usart_write("check update eep NOK\r\n",22,INFO_DEBUG);
		}
	}
}

void check_debug_pool_info(void)
{
	uint8_t recv[100] = {0};
	uint32_t recv_len = 0;
	uint8_t  num_str[5] = {0};

	recv_len = pool_recv_one_command(&debug_pool,recv,100,DEBUG_POOL,4,4);
	if(recv_len > 0)
	{
		if(strcmp((const char *)recv,(const char *)"AT+USECNT\r\n")==0)
		{
			Itoa(use_cnt,num_str);
			Debug_usart_write("AT+USECNT=",10,INFO_DEBUG);
			Debug_usart_write(num_str,str_len(num_str),INFO_DEBUG);
			Debug_usart_write("\r\n",2,INFO_DEBUG);
		}
		else
		{
#if 1
			if(strcmp((const char *)recv,(const char *)"AT+USERST\r\n")==0)
			{
				use_cnt = 0;
				update_use_cnt_to_eeprom(use_cnt);
				Debug_usart_write("AT+USERST=OK\r\n",14,INFO_DEBUG);
			}
#endif
		}
	}
	else
	{
		;//Debug_usart_write("pool nodata\r\n",13,INFO_DEBUG);
	}
}

int main(int argc, char* argv[])
{
	FATFS fs;
	int res;
	uint8_t ret = 0;
	uint8_t flag = 0;
	uint8_t start_count = 0;
	uint32_t succeed_cnt = 0;
	uint8_t buf[2] = {0};

	system_clk_init();

	light_pin_init();
	rst_pin_init();
	isdata_pin_init();
	is_download_pin_init();

	tim_init();
	usart1_init(115200);
	Debug_usart_init();
	iwdg_init();
	I2C_gpio_init();

	initDataPool(&stm32rx);
	initDataPool(&debug_pool);

	NVIC_Configuration();

	res = f_mount(0,&fs);
	if ( res == FR_OK )
	{
		Debug_usart_write("F_mou OK\r\n",10,INFO_DEBUG);
	}
	else
	{
		Debug_usart_write("F_mou NOK\r\n",11,INFO_DEBUG);
	}

	Debug_usart_write("reset\r\n",7,INFO_DEBUG);
	SDcard_log_write((uint8_t*)"software reset\r\n",16,SDCRAD_LOG);

#if 1
	Get_eeprom_data();

	while(1)
	{
		if(cnt_time>=80)
		{
			iwdg_reload();
			//Debug_usart_write("COME IN\r\n",9,INFO_DEBUG);
			//Debug_usart_write(&flag,1,INFO_DEBUG);
			if(flag == 0)
			{
				//if(1)
				if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0)
				{
					start_count++;
					if(start_count == 1)
					{
						flag = 1;
						start_count = 0;
					}
				}
			}
			else if(flag == 1)
			{
				ret = download_start(2250000,ISDATA_PIN_READ);
				Debug_usart_write("baud:2250000\r\n",14,INFO_DEBUG);
				//ret = download_start(1200000,ISDATA_PIN_READ);
				if(ret != 0)
				{
					flag = 0;
					if(ret==1)
					{
						SDcard_log_write((uint8_t*)"download ok\r\n\r\n",15,SDCRAD_LOG);
						succeed_cnt++;
						hex_to_str(buf,succeed_cnt);
						Debug_usart_write("succeed cnt:",12,INFO_DEBUG);
						Debug_usart_write(buf,2,INFO_DEBUG);
						Debug_usart_write("\r\n",2,INFO_DEBUG);
					}
					else if(ret==2)
					{
						succeed_cnt = 0;
						SDcard_log_write((uint8_t*)"download nok\r\n\r\n",16,SDCRAD_LOG);
					}
					else if(ret==3)
					{
						succeed_cnt = 0;
						SDcard_log_write((uint8_t*)"download nodata\r\n\r\n",19,SDCRAD_LOG);
					}
				}
				if(ret != 0)
				{
					use_cnt++;
					update_use_cnt_to_eeprom(use_cnt);
				}
			}
			else
			{
				;
			}
			cnt_time = 0;

		}
		check_debug_pool_info();
	}
#endif
}

void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
