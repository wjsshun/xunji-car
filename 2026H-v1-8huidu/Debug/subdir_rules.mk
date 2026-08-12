################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"C:/TI/ti_cgt_arm_llvm_4.0.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/20407/workspace_ccstheia/2026H-v1-8huidu" -I"C:/Users/20407/workspace_ccstheia/2026H-v1-8huidu/BSP" -I"C:/Users/20407/workspace_ccstheia/2026H-v1-8huidu/Debug" -I"C:/TI/mspm0_sdk_2_11_00_07/source/third_party/CMSIS/Core/Include" -I"C:/TI/mspm0_sdk_2_11_00_07/source" -D__MSPM0G3507__ -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1504919563: ../empty.syscfg
	@echo 'SysConfig - building file: "$<"'
	"C:/TI/sysconfig_1.26.2/sysconfig_cli.bat" -s "C:/TI/mspm0_sdk_2_11_00_07/.metadata/product.json" --script "C:/Users/20407/workspace_ccstheia/2026H-v1-8huidu/empty.syscfg" -o "." --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-1504919563 ../empty.syscfg
device.opt: build-1504919563
device.cmd.genlibs: build-1504919563
ti_msp_dl_config.c: build-1504919563
ti_msp_dl_config.h: build-1504919563
Event.dot: build-1504919563

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"C:/TI/ti_cgt_arm_llvm_4.0.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/20407/workspace_ccstheia/2026H-v1-8huidu" -I"C:/Users/20407/workspace_ccstheia/2026H-v1-8huidu/BSP" -I"C:/Users/20407/workspace_ccstheia/2026H-v1-8huidu/Debug" -I"C:/TI/mspm0_sdk_2_11_00_07/source/third_party/CMSIS/Core/Include" -I"C:/TI/mspm0_sdk_2_11_00_07/source" -D__MSPM0G3507__ -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: C:/TI/mspm0_sdk_2_11_00_07/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"C:/TI/ti_cgt_arm_llvm_4.0.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/20407/workspace_ccstheia/2026H-v1-8huidu" -I"C:/Users/20407/workspace_ccstheia/2026H-v1-8huidu/BSP" -I"C:/Users/20407/workspace_ccstheia/2026H-v1-8huidu/Debug" -I"C:/TI/mspm0_sdk_2_11_00_07/source/third_party/CMSIS/Core/Include" -I"C:/TI/mspm0_sdk_2_11_00_07/source" -D__MSPM0G3507__ -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


