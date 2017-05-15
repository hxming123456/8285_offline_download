#include "bsp_usart.h"

void usart1_init()
{
	USART_InitTypeDef USART_InitStructure;

	usart1_gpio_init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART1,&USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	usart1_nvic_init();

	USART_Cmd(USART1,ENABLE);
}

void usart1_change_baud(uint32_t baud)
{
	USART_InitTypeDef USART_InitStructure;

	USART_Cmd(USART1, DISABLE);

	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART1,&USART_InitStructure);

	USART_Cmd(USART1,ENABLE);

}

void usart1_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void usart1_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void usart1_write(uint8_t *data,uint32_t data_len)
{
	uint32_t i = 0;
	for(i=0;i<data_len;i++)
	{
		USART_SendData(USART1,data[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	}
}

int usart1_read(uint8_t *data)
{
	uint8_t i = 0;
	uint8_t timeout = 0;

	while(1)
	{

		if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)!=RESET)
		{
			data[i] = USART_ReceiveData(USART1);
			i++;
			timeout = 0;
		}
		else
		{
			delay_ms(1);
			timeout++;
			if(timeout>20)
			{
				break;
			}
		}
	}

	return i;
}

void Debug_usart_init()
{
	USART_InitTypeDef USART_InitStructure;

	Debug_usart_gpio_init();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART2,&USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	Debug_usart_nvic_init();

	USART_Cmd(USART2,ENABLE);
}

void Debug_usart_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Debug_usart_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Debug_usart_write(void *data,uint32_t data_len,uint8_t debug_type)
{
	uint8_t * buf = (uint8_t *)data;

	uint32_t i = 0;

	if(DEBUG_FLAG == 'Y')
	{
		if(debug_type == 'Y')
		{
			for(i=0;i<data_len;i++)
			{
				USART_SendData(USART2,buf[i]);
				while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
			}
		}
	}
}

int Debug_usart_read(uint8_t *data)
{
	uint8_t i = 0;
	uint8_t timeout = 0;

	while(1)
	{

		if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)!=RESET)
		{
			data[i] = USART_ReceiveData(USART2);
			i++;
			timeout = 0;
		}
		else
		{
			//delay_ms(1);
			timeout++;
			if(timeout>20)
			{
				break;
			}
		}
	}

	return i;
}

void USART1_IRQHandler(void)
{
	uint8_t c;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		c = USART_ReceiveData(USART1);
		write_one_data_to_datapool(&stm32rx,c);
		//usart2_write(&c,1);
	}
}

void USART2_IRQHandler(void)
{
	uint8_t c;

		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
		{
			USART_ClearITPendingBit(USART2,USART_IT_RXNE);
			c = USART_ReceiveData(USART2);
			//write_one_data_to_datapool(&stm32rx,c);
			usart1_write(&c,1);
		}
}
