#ifndef __BSP_IIC_GPIO_SIMULATE_H
#define __BSP_IIC_GPIO_SIMULATE_H

#include "stm32f10x.h"

#define I2C_SCL_HIGH         GPIO_SetBits(GPIOC,GPIO_Pin_1);
#define I2C_SCL_LOW          GPIO_ResetBits(GPIOC,GPIO_Pin_1);

#define I2C_SDA_HIGH         GPIO_SetBits(GPIOC,GPIO_Pin_0);
#define I2C_SDA_LOW      	 GPIO_ResetBits(GPIOC,GPIO_Pin_0);

#define I2C_SCL_read      	 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)
#define I2C_SDA_read      	 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)

void I2C_gpio_init();
void I2C_Delay(uint32_t time);
void I2C_Start(void);
void I2C_Stop(void);
uint8_t I2C_WaitAck(void);
void I2C_SendByte(uint8_t Byte);
uint8_t I2C_ReadByte(uint8_t ack);

#endif
