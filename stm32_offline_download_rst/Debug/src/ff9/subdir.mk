################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ff9/bsp_sdio_sdcard.c \
../src/ff9/cc936.c \
../src/ff9/diskio.c \
../src/ff9/ff.c 

OBJS += \
./src/ff9/bsp_sdio_sdcard.o \
./src/ff9/cc936.o \
./src/ff9/diskio.o \
./src/ff9/ff.o 

C_DEPS += \
./src/ff9/bsp_sdio_sdcard.d \
./src/ff9/cc936.d \
./src/ff9/diskio.d \
./src/ff9/ff.d 


# Each subdirectory must supply rules for building sources it contributes
src/ff9/%.o: ../src/ff9/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"E:\project\eclipse workspace\stm32_offline_download_rst\src\ff9" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


