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

void light_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	//GPIO_ResetBits(GPIOB,GPIO_Pin_9);
}

int main(int argc, char* argv[])
{
	//FIL fnew;
	FATFS fs;
	int res;
	//uint8_t c;
	//uint8_t buf[98] = {0};
	//uint32_t num;

	system_clk_init();

	light_init();
	delay_init();
	usart1_init();
	Debug_usart_init();
	iwdg_init();
	Debug_usart_write("reset\r\n",7,INFO_DEBUG);

	initDataPool(&stm32rx);

	NVIC_Configuration();

	res = f_mount(0,&fs);
	if ( res == FR_OK )
	{
		//usart2_write("F_mou OK\r\n",10,INFO_DEBUG);
	}
	else
	{
		Debug_usart_write("F_mou NOK\r\n",11,INFO_DEBUG);
	}
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


	while(1)
	{
		if(cnt_time>=400)
		{
			//GPIO_SetBits(GPIOB,GPIO_Pin_9);
			iwdg_reload();
		    download_start();
		    cnt_time = 0;
		    //GPIO_ResetBits(GPIOB,GPIO_Pin_9);
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
