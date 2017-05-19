#include "bsp_wwdg.h"

void wwdg_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	WWDG_SetPrescaler(WWDG_Prescaler_8);

	WWDG_Enable(127);
}

void set_wwdg_counter(int val)
{
	WWDG_SetCounter(val);
}
