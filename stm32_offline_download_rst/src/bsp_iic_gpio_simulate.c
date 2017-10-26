#include "bsp_iic_gpio_simulate.h"

void I2C_gpio_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;//0:SDA 1:SCL
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;

	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void SDA_OUT(void)//C0
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= GPIO_Pin_0;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_Out_OD;
    GPIO_Init(GPIOC, &GPIO_InitStructer);
}

void SDA_IN(void)//C0
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= GPIO_Pin_0;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructer);
}

void I2C_Delay(uint32_t time)//2.5US
{
	uint32_t i,j;

	for(i=0;i<time;i++)
	{
		for(j=0;j<13;j++);//13 2.5us
	}
}

void I2C_Start(void)
{
	I2C_SDA_HIGH
	I2C_Delay(1);
	I2C_SCL_HIGH
	I2C_Delay(1);
	I2C_SDA_LOW
	I2C_Delay(1);
	I2C_SCL_LOW
}

void I2C_Stop(void)
{
	I2C_SCL_LOW
    I2C_SDA_LOW;
	I2C_Delay(1);
	I2C_SCL_HIGH
	I2C_Delay(1);
    I2C_SDA_HIGH;
    I2C_Delay(1);
}

uint8_t I2C_WaitAck(void)  //0:aCK     1:NoAck
{
	int count = 0;


	SDA_IN();
    I2C_SCL_HIGH;
#if 1
    count = 10000;
    while(count-- > 0)
    {
    	if(I2C_SDA_read==0)
    	{
    		I2C_SCL_LOW
			SDA_OUT();
    		return  0;
    	}
    }
#endif
    return 1;
}

void I2C_SendByte(uint8_t Byte)
{
	uint8_t Cnt;

	__set_PRIMASK(1);
	SDA_OUT();
	for(Cnt=0;Cnt<8;Cnt++)
	{
		I2C_SCL_LOW
		I2C_Delay(1);
	    if(Byte&0x80)
	        I2C_SDA_HIGH
	    else
	        I2C_SDA_LOW

		I2C_Delay(1);
	    I2C_SCL_HIGH;
	    I2C_Delay(1);
	    I2C_SCL_LOW;
	    Byte <<= 1;
	}
	__set_PRIMASK(0);
}

uint8_t I2C_ReadByte(uint8_t ack)//0:nck  1:ack
{
	uint8_t mask = 0;
	uint8_t data = 0;

	__set_PRIMASK(1);
	SDA_OUT();

	I2C_SDA_HIGH
	SDA_IN();
	for(mask=0x80;mask!=0;mask>>=1)
	{
		I2C_Delay(1);
		I2C_SCL_HIGH;
		if(I2C_SDA_read==0)
			data &= ~mask;
		else
			data |= mask;
		I2C_Delay(1);
		I2C_SCL_LOW;
	}

	SDA_OUT();
	if(ack == 1)
		I2C_SDA_LOW
	else
		I2C_SDA_HIGH
	I2C_Delay(1);
	I2C_SCL_HIGH;
	I2C_Delay(1);
	I2C_SCL_LOW;

	__set_PRIMASK(0);
	return data;
}
