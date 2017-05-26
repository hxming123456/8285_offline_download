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
#include "8285_download_protocol.h"

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

extern int32_t cnt_time;
#define ISDATA_PIN_READ GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)


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

int main(int argc, char* argv[])
{
	FATFS fs;
	int res;
	uint8_t ret = 0;
	uint8_t flag = 0;
	uint8_t start_count = 0;

	system_clk_init();

	light_pin_init();
	rst_pin_init();
	isdata_pin_init();
	is_download_pin_init();

	tim_init();
	usart1_init();
	Debug_usart_init();
	iwdg_init();


	initDataPool(&stm32rx);

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

	while(1)
	{
		if(cnt_time>=400)
		{
			iwdg_reload();
			//Debug_usart_write(&flag,1,INFO_DEBUG);
			if(flag == 0)
			{
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
				ret = download_start(1200000,0);
				//ret = download_start(1200000,ISDATA_PIN_READ);
				if(ret != 0)
				{
					flag = 0;
					if(ret==1)
					{
						SDcard_log_write((uint8_t*)"download ok\r\n\r\n",15,SDCRAD_LOG);
					}
					else if(ret==2)
					{
						SDcard_log_write((uint8_t*)"download nok\r\n\r\n",16,SDCRAD_LOG);
					}
					else if(ret==3)
					{
						SDcard_log_write((uint8_t*)"download nodata\r\n\r\n",19,SDCRAD_LOG);
					}
				}
			}
			else
			{
				;
			}
			cnt_time = 0;

		}
	}
}

void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
