################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: a64
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Drivers/resourcesmanager/subdir.mk
-include Drivers/STM32MP2xx_HAL_Driver/subdir.mk
-include Drivers/BSP/stpmic2/subdir.mk
-include Drivers/BSP/STM32MP257F-EV1/subdir.mk
-include Common/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := STM32MP257F-EV1_DDR_UTILITIES_A35
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
STM32MP257F-EV1_DDR_UTILITIES_A35.elf \

MAP_FILES += \
STM32MP257F-EV1_DDR_UTILITIES_A35.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STM32MP257F-EV1_DDR_UTILITIES_A35.list \

OBJCOPY_BIN += \
STM32MP257F-EV1_DDR_UTILITIES_A35.bin \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: STM32MP257F-EV1_DDR_UTILITIES_A35.elf secondary-outputs

# Tool invocations
STM32MP257F-EV1_DDR_UTILITIES_A35.elf STM32MP257F-EV1_DDR_UTILITIES_A35.map: $(OBJS) $(USER_OBJS) ../LinkerScriptaa64_sysram_a35_phy_fw_sram1.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	aarch64-none-elf-gcc -o "STM32MP257F-EV1_DDR_UTILITIES_A35.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-a35 -mtune=cortex-a35 -g3 -Og -mstrict-align -Wl,--gc-sections -static -lc -lnosys -T"../LinkerScriptaa64_sysram_a35_phy_fw_sram1.ld" -L../../../ -Wl,-Map="STM32MP257F-EV1_DDR_UTILITIES_A35.map"
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	aarch64-none-elf-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STM32MP257F-EV1_DDR_UTILITIES_A35.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	aarch64-none-elf-objdump -h -S $(EXECUTABLES) > "STM32MP257F-EV1_DDR_UTILITIES_A35.list"
	@echo 'Finished building: $@'
	@echo ' '

STM32MP257F-EV1_DDR_UTILITIES_A35.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	aarch64-none-elf-objcopy  -O binary $(EXECUTABLES) "STM32MP257F-EV1_DDR_UTILITIES_A35.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) STM32MP257F-EV1_DDR_UTILITIES_A35.bin STM32MP257F-EV1_DDR_UTILITIES_A35.elf STM32MP257F-EV1_DDR_UTILITIES_A35.list STM32MP257F-EV1_DDR_UTILITIES_A35.map default.size.stdout
	-@echo ' '

post-build:
	../../../../Scripts/imageheader/postbuild_STM32MP2.sh  "/Users/dann/bin/arm-gnu-toolchain-13.2.Rel1-darwin-arm64-aarch64-none-elf/bin" "STM32MP257F-EV1_DDR_UTILITIES_A35" -c "8-A" -v "2.2"
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified post-build

-include ../makefile.targets
