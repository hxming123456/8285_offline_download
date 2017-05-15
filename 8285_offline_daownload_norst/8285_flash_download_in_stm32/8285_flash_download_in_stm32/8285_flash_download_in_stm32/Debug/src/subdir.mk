################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/8285_download_protocol.c \
../src/Data_pool.c \
../src/_write.c \
../src/bsp_iwdg.c \
../src/bsp_time_delay.c \
../src/bsp_usart.c \
../src/bsp_wwdg.c \
../src/clib.c \
../src/main.c 

OBJS += \
./src/8285_download_protocol.o \
./src/Data_pool.o \
./src/_write.o \
./src/bsp_iwdg.o \
./src/bsp_time_delay.o \
./src/bsp_usart.o \
./src/bsp_wwdg.o \
./src/clib.o \
./src/main.o 

C_DEPS += \
./src/8285_download_protocol.d \
./src/Data_pool.d \
./src/_write.d \
./src/bsp_iwdg.d \
./src/bsp_time_delay.d \
./src/bsp_usart.d \
./src/bsp_wwdg.d \
./src/clib.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


