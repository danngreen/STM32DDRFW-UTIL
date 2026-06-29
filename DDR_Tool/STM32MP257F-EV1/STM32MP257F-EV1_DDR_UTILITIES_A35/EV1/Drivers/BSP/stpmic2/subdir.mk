################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: a64
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/Drivers/BSP/stpmic2/stpmic2.c \
/Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/Drivers/BSP/stpmic2/stpmic2_reg.c 

OBJS += \
./Drivers/BSP/stpmic2/stpmic2.o \
./Drivers/BSP/stpmic2/stpmic2_reg.o 

C_DEPS += \
./Drivers/BSP/stpmic2/stpmic2.d \
./Drivers/BSP/stpmic2/stpmic2_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/stpmic2/stpmic2.o: /Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/Drivers/BSP/stpmic2/stpmic2.c Drivers/BSP/stpmic2/subdir.mk
	aarch64-none-elf-gcc "$<" -mcpu=cortex-a35 -g3 -D__AARCH64__ -DSTM32MP257Cxx -DSTM32MP25xxxx -DCORE_CA35 -DCORTEX_IN_SECURE_STATE -DUSE_STM32MP257F_EV1 -DUSE_DEVELOPMENT_MODE -DUSE_BSP_V2 -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -DUSE_IOEXPANDER -D__LOG_UART_IO_ -DLOGLEVEL=LOGINFO -DDDR_SIZE_Gb=32 -DDDR_FREQ=1200 -DSTM32MP_DDR_DUAL_AXI_PORT=1 -DDDR_INTERACTIVE -DSTM32MP_DDR3_TYPE=0 -DSTM32MP_DDR4_TYPE=1 -DSTM32MP_LPDDR4_TYPE=0 -c -I../../Inc -I../../../Common/Inc -I../../../Common_MP2/Inc -I../../../../Drivers/CMSIS/MP2/Core_A/Include -I../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include -I../../../../Drivers/STM32MP2xx_HAL_Driver/Inc -I../../../../Drivers/BSP/STM32MP257F-EV1 -I../../../../Drivers/BSP/stpmic2 -I../../../../Scripts/resourcesmanager -Og -ffunction-sections -Wall -Wextra -mtune=cortex-a35 -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -o "$@"
Drivers/BSP/stpmic2/stpmic2_reg.o: /Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/Drivers/BSP/stpmic2/stpmic2_reg.c Drivers/BSP/stpmic2/subdir.mk
	aarch64-none-elf-gcc "$<" -mcpu=cortex-a35 -g3 -D__AARCH64__ -DSTM32MP257Cxx -DSTM32MP25xxxx -DCORE_CA35 -DCORTEX_IN_SECURE_STATE -DUSE_STM32MP257F_EV1 -DUSE_DEVELOPMENT_MODE -DUSE_BSP_V2 -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -DUSE_IOEXPANDER -D__LOG_UART_IO_ -DLOGLEVEL=LOGINFO -DDDR_SIZE_Gb=32 -DDDR_FREQ=1200 -DSTM32MP_DDR_DUAL_AXI_PORT=1 -DDDR_INTERACTIVE -DSTM32MP_DDR3_TYPE=0 -DSTM32MP_DDR4_TYPE=1 -DSTM32MP_LPDDR4_TYPE=0 -c -I../../Inc -I../../../Common/Inc -I../../../Common_MP2/Inc -I../../../../Drivers/CMSIS/MP2/Core_A/Include -I../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include -I../../../../Drivers/STM32MP2xx_HAL_Driver/Inc -I../../../../Drivers/BSP/STM32MP257F-EV1 -I../../../../Drivers/BSP/stpmic2 -I../../../../Scripts/resourcesmanager -Og -ffunction-sections -Wall -Wextra -mtune=cortex-a35 -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -o "$@"

clean: clean-Drivers-2f-BSP-2f-stpmic2

clean-Drivers-2f-BSP-2f-stpmic2:
	-$(RM) ./Drivers/BSP/stpmic2/stpmic2.cyclo ./Drivers/BSP/stpmic2/stpmic2.d ./Drivers/BSP/stpmic2/stpmic2.o ./Drivers/BSP/stpmic2/stpmic2.su ./Drivers/BSP/stpmic2/stpmic2_reg.cyclo ./Drivers/BSP/stpmic2/stpmic2_reg.d ./Drivers/BSP/stpmic2/stpmic2_reg.o ./Drivers/BSP/stpmic2/stpmic2_reg.su

.PHONY: clean-Drivers-2f-BSP-2f-stpmic2

