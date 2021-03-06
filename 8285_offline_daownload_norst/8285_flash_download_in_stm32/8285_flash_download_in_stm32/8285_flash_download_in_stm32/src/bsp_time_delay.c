#include "bsp_time_delay.h"

int32_t cnt_time=0;
int32_t recv_time_out = 0;
int32_t data_cnt = 0;
int32_t sync_time = 0;
int32_t nodata_time = 0;
extern uint8_t sync_flag;
extern uint8_t nodata_flag;
extern uint8_t wait_baud_flag;

void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 1000;

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
		if((stm32rx.stock>=0) && (stm32rx.stock!=data_cnt))
		{
			data_cnt = stm32rx.stock;
		}
		else if((stm32rx.stock>10) || sync_flag==1 || wait_baud_flag==1)
		{
			if(recv_time_out != 0)
			{
				recv_time_out--;
				if(recv_time_out==0)
				{
					stm32rx.avail = 1;
				}
			}
		}
#endif

		if(cnt_time < 400)
		{
			cnt_time++;
		}
		if(sync_flag)
		{
			sync_time++;
			if(sync_time >= 200)
			{
				sync_time = 0;
				GPIOB->ODR ^= GPIO_Pin_12;
			}
		}
		if(nodata_flag)
		{
			nodata_time++;
			if(nodata_time >= 200)
			{
				nodata_time = 0;
				GPIOB->ODR ^= GPIO_Pin_14;
			}
		}
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);

	}
}

void delay_init(void)
{
	TIM2_Configuration();
	TIM2_NVIC_Configuration();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
}

void delay_ms(uint32_t mscnt)
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

void delay_s(uint32_t scnt)
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
