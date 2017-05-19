#include "bsp_iwdg.h"

/**
 * ��ʼ���������Ź�
 * prer:��Ƶ��:0~7(ֻ�е� 3 λ��Ч!)
 * ��Ƶ����=4*2^prer.�����ֵֻ���� 256!
 * rlr:��װ�ؼĴ���ֵ:�� 11 λ��Ч.
 * ʱ�����(���):Tout=((4*2^prer)*rlr)/40 (ms).
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
