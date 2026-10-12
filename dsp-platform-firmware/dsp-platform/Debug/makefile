################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/gcc-arm-none-eabi_9_3_1

GEN_OPTS__FLAG := @"syscfg/ti_utils_build_compiler.opt" 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./dds.o" \
"./dsp.o" \
"./empty.o" \
"./syscfg/ti_devices_config.o" \
"./syscfg/ti_drivers_config.o" \
"./syscfg/ti_freertos_config.o" \
"./syscfg/ti_freertos_portable_config.o" \
"./main_freertos.o" \
"./remote.o" \
"./serial_rpc.o" \
-Wl,-T"../lpf3_cc23xx_freertos.lds" \
$(GEN_CMDS__FLAG) \
-l:ti_utils_build_linker.cmd.genlibs \
-lgcc \
-lc \
-lm \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
dsp-platform.out 

EXE_OUTPUTS__QUOTED += \
"dsp-platform.out" 


# All Target
all: $(OBJS) $(LDS_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "dsp-platform.out"

# Tool invocations
dsp-platform.out: $(OBJS) $(LDS_SRCS) $(GEN_CMDS)
	@echo 'GNU Linker - building target: "$@"'
	"C:/ti/gcc-arm-none-eabi_9_3_1/bin/arm-none-eabi-gcc-9.3.1.exe" -D_REENT_SMALL -O3 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -mthumb -mfloat-abi=soft -Wl,-Map,"dsp-platform.map" -nostartfiles -static -Wl,--gc-sections -L"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/source" -L"F:/content_writing/dsp-platform/dsp-platform-firmware/dsp-platform/Debug/syscfg" -L"C:/ti/gcc-arm-none-eabi_9_3_1/arm-none-eabi/lib/thumb/v6-m/nofp" -mcpu=cortex-m0plus -fno-lto -march=armv6-m -mthumb --specs=nano.specs -o"dsp-platform.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "dds.o" "dsp.o" "empty.o" "syscfg\ti_devices_config.o" "syscfg\ti_drivers_config.o" "syscfg\ti_freertos_config.o" "syscfg\ti_freertos_portable_config.o" "main_freertos.o" "remote.o" "serial_rpc.o" 
	-$(RM) "dds.d" "dsp.d" "empty.d" "syscfg\ti_devices_config.d" "syscfg\ti_drivers_config.d" "syscfg\ti_freertos_config.d" "syscfg\ti_freertos_portable_config.d" "main_freertos.d" "remote.d" "serial_rpc.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo ' '

post-build:
	-"C:/ti/simplelink_lowpower_f3_sdk_9_14_01_16/tools/common/crc_tool/crc_tool" patch-image --elf "F:/content_writing/dsp-platform/dsp-platform-firmware/dsp-platform/Debug/dsp-platform.out" --symbol-prefix ti_utils_build_GenMap_sym_CRC --output "F:/content_writing/dsp-platform/dsp-platform-firmware/dsp-platform/Debug/dsp-platform.out"
	-"C:/ti/gcc-arm-none-eabi_9_3_1/bin/arm-none-eabi-objcopy" -O ihex "F:/content_writing/dsp-platform/dsp-platform-firmware/dsp-platform/Debug/dsp-platform.out" "F:/content_writing/dsp-platform/dsp-platform-firmware/dsp-platform/Debug/dsp-platform.hex"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

