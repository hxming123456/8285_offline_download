#include "bsp_eeprom.h"

uint8_t eep_write_data(uint8_t *data,uint8_t count,uint8_t addr)
{
	uint8_t i = 0;;

	I2C_Start();
	I2C_SendByte(0xa0);
	if(I2C_WaitAck())
	{
		Debug_usart_write("send dev addr error\r\n",21,INFO_DEBUG);
	    return 1;
	}
	I2C_SendByte(addr);
	if(I2C_WaitAck())
	{
		Debug_usart_write("send word addr error\r\n",22,INFO_DEBUG);
	    return 2;
	}
	for (i = 0; i < count; i++)
	{
		I2C_SendByte(data[i]);
		if(I2C_WaitAck())
		{
			Debug_usart_write("send data error\r\n",17,INFO_DEBUG);
		    return 3;
		}
	}

	I2C_Stop();
	I2C_Delay(1000);
	return 0;
}

uint8_t eep_read_data(uint8_t *data,uint8_t count,uint8_t addr)
{
	I2C_Start();
	I2C_SendByte(0xa0);
	if(I2C_WaitAck())
	{
		Debug_usart_write((void *)"send dev addr error\r\n",21,INFO_DEBUG);
		return 1;
	}
	I2C_SendByte(addr);
	if(I2C_WaitAck())
	{
		Debug_usart_write((void *)"send word addr error\r\n",22,INFO_DEBUG);
	    return 2;
	}

	I2C_Start();
	I2C_SendByte(0xa1);
	if(I2C_WaitAck())
	{
		Debug_usart_write((void *)"send dev addr again error\r\n",27,INFO_DEBUG);
		return 1;
	}

	while(count > 1)
	{
		*data++ = I2C_ReadByte(1);
		count--;
	}

	*data = I2C_ReadByte(0);
	I2C_Stop();
	I2C_Delay(1000);
	return 0;
}
