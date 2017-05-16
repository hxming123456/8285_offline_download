#include "bsp_iwdg.h"

/**
 * 初始化独立看门狗
 * prer:分频数:0~7(只有低 3 位有效!)
 * 分频因子=4*2^prer.但最大值只能是 256!
 * rlr:重装载寄存器值:低 11 位有效.
 * 时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
 */

void iwdg_init()
{
	uint8_t pre = 4;
	int rlr = 1875;
	RCC_LSICmd(ENABLE);

	  /* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{}

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(pre);
	IWDG_SetReload(rlr);
	IWDG_ReloadCounter();
	IWDG_Enable();
}

void iwdg_reload()
{
	IWDG_ReloadCounter();
}
