################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Custom_External_HAL_Driver/Src/lcd.c 

OBJS += \
./Drivers/Custom_External_HAL_Driver/Src/lcd.o 

C_DEPS += \
./Drivers/Custom_External_HAL_Driver/Src/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Custom_External_HAL_Driver/Src/%.o Drivers/Custom_External_HAL_Driver/Src/%.su Drivers/Custom_External_HAL_Driver/Src/%.cyclo: ../Drivers/Custom_External_HAL_Driver/Src/%.c Drivers/Custom_External_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"D:/Documents/training materials/enrichtraining_rtos/my_workspace/digital-watch-project/Drivers/Custom_External_HAL_Driver/Inc" -I"D:/Documents/training materials/enrichtraining_rtos/my_workspace/digital-watch-project/FreeRTOS-Kernel/include" -I"D:/Documents/training materials/enrichtraining_rtos/my_workspace/digital-watch-project/FreeRTOS-Kernel/portable/GCC/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Custom_External_HAL_Driver-2f-Src

clean-Drivers-2f-Custom_External_HAL_Driver-2f-Src:
	-$(RM) ./Drivers/Custom_External_HAL_Driver/Src/lcd.cyclo ./Drivers/Custom_External_HAL_Driver/Src/lcd.d ./Drivers/Custom_External_HAL_Driver/Src/lcd.o ./Drivers/Custom_External_HAL_Driver/Src/lcd.su

.PHONY: clean-Drivers-2f-Custom_External_HAL_Driver-2f-Src

