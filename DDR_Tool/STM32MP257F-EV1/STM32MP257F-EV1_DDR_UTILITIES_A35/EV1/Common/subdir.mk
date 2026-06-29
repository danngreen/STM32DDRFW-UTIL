################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: a64
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common/Src/syscalls.c 

OBJS += \
./Common/syscalls.o 

C_DEPS += \
./Common/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
Common/syscalls.o: /Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common/Src/syscalls.c Common/subdir.mk
	aarch64-none-elf-gcc "$<" -mcpu=cortex-a35 -g3 -D__AARCH64__ -DSTM32MP257Cxx -DSTM32MP25xxxx -DCORE_CA35 -DCORTEX_IN_SECURE_STATE -DUSE_STM32MP257F_EV1 -DUSE_DEVELOPMENT_MODE -DUSE_BSP_V2 -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -DUSE_IOEXPANDER -D__LOG_UART_IO_ -DLOGLEVEL=LOGINFO -DDDR_SIZE_Gb=32 -DDDR_FREQ=1200 -DSTM32MP_DDR_DUAL_AXI_PORT=1 -DDDR_INTERACTIVE -DSTM32MP_DDR3_TYPE=0 -DSTM32MP_DDR4_TYPE=1 -DSTM32MP_LPDDR4_TYPE=0 -c -I../../Inc -I../../../Common/Inc -I../../../Common_MP2/Inc -I../../../../Drivers/CMSIS/MP2/Core_A/Include -I../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include -I../../../../Drivers/STM32MP2xx_HAL_Driver/Inc -I../../../../Drivers/BSP/STM32MP257F-EV1 -I../../../../Drivers/BSP/stpmic2 -I../../../../Scripts/resourcesmanager -Og -ffunction-sections -Wall -Wextra -mtune=cortex-a35 -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -o "$@"

clean: clean-Common

clean-Common:
	-$(RM) ./Common/syscalls.cyclo ./Common/syscalls.d ./Common/syscalls.o ./Common/syscalls.su

.PHONY: clean-Common

