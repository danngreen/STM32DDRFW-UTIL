################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: a64
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
/Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/minimal_startup64_a35.s 

C_SRCS += \
/Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/ddr_tests.c \
/Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/ddr_tool.c \
/Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/STM32MP257F-EV1/Src/ddr_tool_util.c \
/Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/main.c \
/Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/stm32mp2xx_hal_msp.c \
/Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/stm32mp2xx_it.c 

OBJS += \
./User/ddr_tests.o \
./User/ddr_tool.o \
./User/ddr_tool_util.o \
./User/main.o \
./User/minimal_startup64_a35.o \
./User/stm32mp2xx_hal_msp.o \
./User/stm32mp2xx_it.o 

S_DEPS += \
./User/minimal_startup64_a35.d 

C_DEPS += \
./User/ddr_tests.d \
./User/ddr_tool.d \
./User/ddr_tool_util.d \
./User/main.d \
./User/stm32mp2xx_hal_msp.d \
./User/stm32mp2xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
User/ddr_tests.o: /Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/ddr_tests.c User/subdir.mk
	aarch64-none-elf-gcc "$<" -mcpu=cortex-a35 -g3 -D__AARCH64__ -DSTM32MP257Cxx -DSTM32MP25xxxx -DCORE_CA35 -DCORTEX_IN_SECURE_STATE -DUSE_STM32MP257F_EV1 -DUSE_DEVELOPMENT_MODE -DUSE_BSP_V2 -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -DUSE_IOEXPANDER -D__LOG_UART_IO_ -DLOGLEVEL=LOGINFO -DDDR_SIZE_Gb=32 -DDDR_FREQ=1200 -DSTM32MP_DDR_DUAL_AXI_PORT=1 -DDDR_INTERACTIVE -DSTM32MP_DDR3_TYPE=0 -DSTM32MP_DDR4_TYPE=1 -DSTM32MP_LPDDR4_TYPE=0 -c -I../../Inc -I../../../Common/Inc -I../../../Common_MP2/Inc -I../../../../Drivers/CMSIS/MP2/Core_A/Include -I../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include -I../../../../Drivers/STM32MP2xx_HAL_Driver/Inc -I../../../../Drivers/BSP/STM32MP257F-EV1 -I../../../../Drivers/BSP/stpmic2 -I../../../../Scripts/resourcesmanager -Og -ffunction-sections -Wall -Wextra -mtune=cortex-a35 -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -o "$@"
User/ddr_tool.o: /Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/ddr_tool.c User/subdir.mk
	aarch64-none-elf-gcc "$<" -mcpu=cortex-a35 -g3 -D__AARCH64__ -DSTM32MP257Cxx -DSTM32MP25xxxx -DCORE_CA35 -DCORTEX_IN_SECURE_STATE -DUSE_STM32MP257F_EV1 -DUSE_DEVELOPMENT_MODE -DUSE_BSP_V2 -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -DUSE_IOEXPANDER -D__LOG_UART_IO_ -DLOGLEVEL=LOGINFO -DDDR_SIZE_Gb=32 -DDDR_FREQ=1200 -DSTM32MP_DDR_DUAL_AXI_PORT=1 -DDDR_INTERACTIVE -DSTM32MP_DDR3_TYPE=0 -DSTM32MP_DDR4_TYPE=1 -DSTM32MP_LPDDR4_TYPE=0 -c -I../../Inc -I../../../Common/Inc -I../../../Common_MP2/Inc -I../../../../Drivers/CMSIS/MP2/Core_A/Include -I../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include -I../../../../Drivers/STM32MP2xx_HAL_Driver/Inc -I../../../../Drivers/BSP/STM32MP257F-EV1 -I../../../../Drivers/BSP/stpmic2 -I../../../../Scripts/resourcesmanager -Og -ffunction-sections -Wall -Wextra -mtune=cortex-a35 -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -o "$@"
User/ddr_tool_util.o: /Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/STM32MP257F-EV1/Src/ddr_tool_util.c User/subdir.mk
	aarch64-none-elf-gcc "$<" -mcpu=cortex-a35 -g3 -D__AARCH64__ -DSTM32MP257Cxx -DSTM32MP25xxxx -DCORE_CA35 -DCORTEX_IN_SECURE_STATE -DUSE_STM32MP257F_EV1 -DUSE_DEVELOPMENT_MODE -DUSE_BSP_V2 -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -DUSE_IOEXPANDER -D__LOG_UART_IO_ -DLOGLEVEL=LOGINFO -DDDR_SIZE_Gb=32 -DDDR_FREQ=1200 -DSTM32MP_DDR_DUAL_AXI_PORT=1 -DDDR_INTERACTIVE -DSTM32MP_DDR3_TYPE=0 -DSTM32MP_DDR4_TYPE=1 -DSTM32MP_LPDDR4_TYPE=0 -c -I../../Inc -I../../../Common/Inc -I../../../Common_MP2/Inc -I../../../../Drivers/CMSIS/MP2/Core_A/Include -I../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include -I../../../../Drivers/STM32MP2xx_HAL_Driver/Inc -I../../../../Drivers/BSP/STM32MP257F-EV1 -I../../../../Drivers/BSP/stpmic2 -I../../../../Scripts/resourcesmanager -Og -ffunction-sections -Wall -Wextra -mtune=cortex-a35 -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -o "$@"
User/main.o: /Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/main.c User/subdir.mk
	aarch64-none-elf-gcc "$<" -mcpu=cortex-a35 -g3 -D__AARCH64__ -DSTM32MP257Cxx -DSTM32MP25xxxx -DCORE_CA35 -DCORTEX_IN_SECURE_STATE -DUSE_STM32MP257F_EV1 -DUSE_DEVELOPMENT_MODE -DUSE_BSP_V2 -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -DUSE_IOEXPANDER -D__LOG_UART_IO_ -DLOGLEVEL=LOGINFO -DDDR_SIZE_Gb=32 -DDDR_FREQ=1200 -DSTM32MP_DDR_DUAL_AXI_PORT=1 -DDDR_INTERACTIVE -DSTM32MP_DDR3_TYPE=0 -DSTM32MP_DDR4_TYPE=1 -DSTM32MP_LPDDR4_TYPE=0 -c -I../../Inc -I../../../Common/Inc -I../../../Common_MP2/Inc -I../../../../Drivers/CMSIS/MP2/Core_A/Include -I../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include -I../../../../Drivers/STM32MP2xx_HAL_Driver/Inc -I../../../../Drivers/BSP/STM32MP257F-EV1 -I../../../../Drivers/BSP/stpmic2 -I../../../../Scripts/resourcesmanager -Og -ffunction-sections -Wall -Wextra -mtune=cortex-a35 -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -o "$@"
User/minimal_startup64_a35.o: /Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/minimal_startup64_a35.s User/subdir.mk
	aarch64-none-elf-gcc -mcpu=cortex-a35 -mtune=cortex-a35 -g3 -Og -mstrict-align -fshort-enums -Wall -Wextra -D__AARCH64__ -c  -o "$@" "$<"
User/stm32mp2xx_hal_msp.o: /Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/stm32mp2xx_hal_msp.c User/subdir.mk
	aarch64-none-elf-gcc "$<" -mcpu=cortex-a35 -g3 -D__AARCH64__ -DSTM32MP257Cxx -DSTM32MP25xxxx -DCORE_CA35 -DCORTEX_IN_SECURE_STATE -DUSE_STM32MP257F_EV1 -DUSE_DEVELOPMENT_MODE -DUSE_BSP_V2 -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -DUSE_IOEXPANDER -D__LOG_UART_IO_ -DLOGLEVEL=LOGINFO -DDDR_SIZE_Gb=32 -DDDR_FREQ=1200 -DSTM32MP_DDR_DUAL_AXI_PORT=1 -DDDR_INTERACTIVE -DSTM32MP_DDR3_TYPE=0 -DSTM32MP_DDR4_TYPE=1 -DSTM32MP_LPDDR4_TYPE=0 -c -I../../Inc -I../../../Common/Inc -I../../../Common_MP2/Inc -I../../../../Drivers/CMSIS/MP2/Core_A/Include -I../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include -I../../../../Drivers/STM32MP2xx_HAL_Driver/Inc -I../../../../Drivers/BSP/STM32MP257F-EV1 -I../../../../Drivers/BSP/stpmic2 -I../../../../Scripts/resourcesmanager -Og -ffunction-sections -Wall -Wextra -mtune=cortex-a35 -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -o "$@"
User/stm32mp2xx_it.o: /Users/dann/4ms/stm32/mp2-dev/STM32DDRFW-UTIL/DDR_Tool/Common_MP2/Src/stm32mp2xx_it.c User/subdir.mk
	aarch64-none-elf-gcc "$<" -mcpu=cortex-a35 -g3 -D__AARCH64__ -DSTM32MP257Cxx -DSTM32MP25xxxx -DCORE_CA35 -DCORTEX_IN_SECURE_STATE -DUSE_STM32MP257F_EV1 -DUSE_DEVELOPMENT_MODE -DUSE_BSP_V2 -DUSE_HAL_DRIVER -DUSE_FULL_LL_DRIVER -DUSE_IOEXPANDER -D__LOG_UART_IO_ -DLOGLEVEL=LOGINFO -DDDR_SIZE_Gb=32 -DDDR_FREQ=1200 -DSTM32MP_DDR_DUAL_AXI_PORT=1 -DDDR_INTERACTIVE -DSTM32MP_DDR3_TYPE=0 -DSTM32MP_DDR4_TYPE=1 -DSTM32MP_LPDDR4_TYPE=0 -c -I../../Inc -I../../../Common/Inc -I../../../Common_MP2/Inc -I../../../../Drivers/CMSIS/MP2/Core_A/Include -I../../../../Drivers/CMSIS/Device/ST/STM32MP2xx/Include -I../../../../Drivers/STM32MP2xx_HAL_Driver/Inc -I../../../../Drivers/BSP/STM32MP257F-EV1 -I../../../../Drivers/BSP/stpmic2 -I../../../../Scripts/resourcesmanager -Og -ffunction-sections -Wall -Wextra -mtune=cortex-a35 -fshort-enums -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -o "$@"

clean: clean-User

clean-User:
	-$(RM) ./User/ddr_tests.cyclo ./User/ddr_tests.d ./User/ddr_tests.o ./User/ddr_tests.su ./User/ddr_tool.cyclo ./User/ddr_tool.d ./User/ddr_tool.o ./User/ddr_tool.su ./User/ddr_tool_util.cyclo ./User/ddr_tool_util.d ./User/ddr_tool_util.o ./User/ddr_tool_util.su ./User/main.cyclo ./User/main.d ./User/main.o ./User/main.su ./User/minimal_startup64_a35.d ./User/minimal_startup64_a35.o ./User/stm32mp2xx_hal_msp.cyclo ./User/stm32mp2xx_hal_msp.d ./User/stm32mp2xx_hal_msp.o ./User/stm32mp2xx_hal_msp.su ./User/stm32mp2xx_it.cyclo ./User/stm32mp2xx_it.d ./User/stm32mp2xx_it.o ./User/stm32mp2xx_it.su

.PHONY: clean-User

