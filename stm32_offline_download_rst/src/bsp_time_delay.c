#include "bsp_time_delay.h"

int32_t cnt_time=0;
uint32_t recv_time_out = 0;
int32_t data_cnt = 0;
int32_t sync_time = 0;
int32_t nodata_time = 0;

extern uint8_t nodata_flag;
extern uint8_t update_status;
extern uint32_t recv_time_out;
extern int32_t cnt_time;
extern uint8_t wait_baud_flag;
extern uint32_t usart_send_timeout;
extern uint8_t all_time_flag;
extern uint32_t send_time;
extern uint8_t recv_over_flag;

void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 5000;

	TIM_TimeBaseStructure.TIM_Prescaler = 71;

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;

	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

    TIM_Cmd(TIM2, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
}

void TIM2_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET )
	{
#if 1
		if(recv_time_out != 0)
		{
			if(stm32rx.stock>=8 || wait_baud_flag==1 || sync_flag == 1)
			{
				recv_time_out--;
				if(recv_time_out==0)
				{
					recv_over_flag = 1;
					Debug_usart_write("out 1\r\n",7,INFO_DEBUG);
				}
			}
		}

#else
		if(all_time_flag==0)
		{
			send_time++;
			if(send_time>=200)
			{
				all_time_flag = 1;
				Debug_usart_write("out 200ms\r\n",10,INFO_DEBUG);
			}
		}
#endif
		change_light_status(update_status);
		if(cnt_time < 80)
		{
			cnt_time++;
		}
		if(usart_send_timeout > 0)
		{
			usart_send_timeout--;
		}
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);

	}
}

void change_light_status(uint8_t status)
{
	switch(status)
	{
		case DOWNLOAD_OK_STATUS:
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			GPIO_SetBits(GPIOB,GPIO_Pin_15);
			break;
		case DOWNLOAD_NOK_STATUS:
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_SetBits(GPIOB,GPIO_Pin_15);
			break;
		case DOWNLOAD_NOW_STATUS:
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			break;
		case NO_DATA_STATUS:
			GPIO_SetBits(GPIOB,GPIO_Pin_15);
			if(nodata_flag)
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_13);
				GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			}
			break;
		default:break;
	}



}

void tim_init(void)
{
	TIM2_Configuration();
	TIM2_NVIC_Configuration();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
}

void delay_ms(int32_t mscnt)
{
	cnt_time = 0;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

	while(1)
	{
		if(cnt_time==mscnt)
		{
			break;
		}
	}

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
}

void delay_s(int32_t scnt)
{
	cnt_time = 0;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

	while(1)
	{
		if(cnt_time==(scnt*1000))
		{
			break;
		}
	}

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
}
