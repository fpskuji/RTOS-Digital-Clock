################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeRTOS-Kernel/examples/cmake_example/main.c 

OBJS += \
./FreeRTOS-Kernel/examples/cmake_example/main.o 

C_DEPS += \
./FreeRTOS-Kernel/examples/cmake_example/main.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS-Kernel/examples/cmake_example/%.o FreeRTOS-Kernel/examples/cmake_example/%.su FreeRTOS-Kernel/examples/cmake_example/%.cyclo: ../FreeRTOS-Kernel/examples/cmake_example/%.c FreeRTOS-Kernel/examples/cmake_example/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"D:/Documents/training materials/enrichtraining_rtos/my_workspace/digital-watch-project/FreeRTOS-Kernel/include" -I"D:/Documents/training materials/enrichtraining_rtos/my_workspace/digital-watch-project/FreeRTOS-Kernel/portable/GCC/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FreeRTOS-2d-Kernel-2f-examples-2f-cmake_example

clean-FreeRTOS-2d-Kernel-2f-examples-2f-cmake_example:
	-$(RM) ./FreeRTOS-Kernel/examples/cmake_example/main.cyclo ./FreeRTOS-Kernel/examples/cmake_example/main.d ./FreeRTOS-Kernel/examples/cmake_example/main.o ./FreeRTOS-Kernel/examples/cmake_example/main.su

.PHONY: clean-FreeRTOS-2d-Kernel-2f-examples-2f-cmake_example

