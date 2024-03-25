################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include serial/subdir.mk
-include emlib/subdir.mk
-include emdrv/subdir.mk
-include Source/Mid/button/subdir.mk
-include Source/Mid/Led/subdir.mk
-include Source/Mid/LM75/subdir.mk
-include Source/Mid/LCD_20_4/subdir.mk
-include Source/Mid/Kalman_Fillter/subdir.mk
-include Source/Mid/BH1750/subdir.mk
-include Source/Driver/subdir.mk
-include Source/App/Send/subdir.mk
-include Source/App/Receive/subdir.mk
-include Source/App/Network/subdir.mk
-include Source/App/Main/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: pre-build main-build

# Main-build Target
main-build: Project_ZB_Cordinator.axf

# Tool invocations
Project_ZB_Cordinator.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m33 -mthumb -T "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/base/hal/micro/cortexm3/efm32/gcc-cfg.ld" -L"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/base/hal/micro/cortexm3/" -Xlinker --defsym=FLASH_SIZE=524288 -Xlinker --defsym=RAM_SIZE=65536 -Xlinker --defsym=FLASH_PAGE_SIZE=8192 -Xlinker --defsym=SIMEEPROM_SIZE=32768 -Xlinker --defsym=PSSTORE_SIZE=0 -Xlinker --defsym=LONGTOKEN_SIZE=0 -Xlinker --defsym=HEADER_SIZE=512 -Xlinker --defsym=BTL_SIZE=16384 -Xlinker --defsym=LOCKBITS_IN_MAINFLASH_SIZE=8192 -Xlinker --defsym=NULL_BTL=1 -Xlinker --defsym=EMBER_MALLOC_HEAP_SIZE=0 -Xlinker --gc-sections -Xlinker -Map="Project_ZB_Cordinator.map" -mfpu=fpv5-sp-d16 -mfloat-abi=hard --specs=nano.specs -o Project_ZB_Cordinator.axf -Wl,--start-group "./Project_ZB_Cordinator_callbacks.o" "./call-command-handler.o" "./callback-stub.o" "./command-interpreter2.o" "./ember-configuration.o" "./library.o" "./security-address-cache.o" "./stack-handler-stub.o" "./zigbee-device-common.o" "./zigbee-device-library.o" "./znet-bookkeeping.o" "./znet-cli.o" "./Source/App/Main/Ringbuffer.o" "./Source/App/Main/main.o" "./Source/App/Network/Network.o" "./Source/App/Receive/Receive.o" "./Source/App/Send/Send.o" "./Source/Driver/file - Copy (2).o" "./Source/Driver/file - Copy (3).o" "./Source/Driver/file.o" "./Source/Driver/gpio.o" "./Source/Driver/i2c.o" "./Source/Driver/pwm.o" "./Source/Driver/spi.o" "./Source/Driver/uart.o" "./Source/Mid/BH1750/bh1750.o" "./Source/Mid/Kalman_Fillter/filter.o" "./Source/Mid/LCD_20_4/LCD_I2C.o" "./Source/Mid/LM75/LM75.o" "./Source/Mid/Led/led.o" "./Source/Mid/button/button.o" "./emdrv/uartdrv.o" "./emlib/system_efr32mg21.o" "./serial/com.o" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/source-route-library-cortexm3-gcc-efr32mg21-rail/source-route-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/debug-basic-stub-library-cortexm3-gcc-efr32mg21-rail/debug-basic-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/end-device-bind-stub-library-cortexm3-gcc-efr32mg21-rail/end-device-bind-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/sim-eeprom2-to-nvm3-upgrade-stub-library-cortexm3-gcc-efr32mg21-rail/sim-eeprom2-to-nvm3-upgrade-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/cbke-283k1-stub-library-cortexm3-gcc-efr32mg21-rail/cbke-283k1-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/debug-extended-stub-library-cortexm3-gcc-efr32mg21-rail/debug-extended-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/cbke-library-dsa-verify-stub-cortexm3-gcc-efr32mg21-rail/cbke-library-dsa-verify-stub.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/radio/rail_lib/autogen/librail_release/librail_efr32xg21_gcc_release.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/emdrv/nvm3/lib/libnvm3_CM33_gcc.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/cbke-stub-library-cortexm3-gcc-efr32mg21-rail/cbke-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/hal-library-cortexm3-gcc-efr32mg21-rail/hal-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/packet-validate-stub-library-cortexm3-gcc-efr32mg21-rail/packet-validate-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/security-library-link-keys-stub-cortexm3-gcc-efr32mg21-rail/security-library-link-keys-stub.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/binding-table-library-cortexm3-gcc-efr32mg21-rail/binding-table-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/cbke-163k1-stub-library-cortexm3-gcc-efr32mg21-rail/cbke-163k1-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/cbke-library-dsa-sign-stub-cortexm3-gcc-efr32mg21-rail/cbke-library-dsa-sign-stub.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/gp-stub-library-cortexm3-gcc-efr32mg21-rail/gp-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/multi-network-stub-library-cortexm3-gcc-efr32mg21-rail/multi-network-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/zigbee-pro-stack-cortexm3-gcc-efr32mg21-rail/zigbee-pro-stack.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/cbke-library-dsa-verify-283k1-stub-cortexm3-gcc-efr32mg21-rail/cbke-library-dsa-verify-283k1-stub.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/zll-stub-library-cortexm3-gcc-efr32mg21-rail/zll-stub-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/install-code-library-cortexm3-gcc-efr32mg21-rail/install-code-library.a" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//protocol/zigbee/build/zigbee-r22-support-stub-library-cortexm3-gcc-efr32mg21-rail/zigbee-r22-support-stub-library.a" -lm -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: Project_ZB_Cordinator.hex'
	arm-none-eabi-objcopy -O ihex "Project_ZB_Cordinator.axf" "Project_ZB_Cordinator.hex"
	@echo ' '

	@echo 'Building bin file: Project_ZB_Cordinator.bin'
	arm-none-eabi-objcopy -O binary "Project_ZB_Cordinator.axf" "Project_ZB_Cordinator.bin"
	@echo ' '

	@echo 'Building s37 file: Project_ZB_Cordinator.s37'
	arm-none-eabi-objcopy -O srec "Project_ZB_Cordinator.axf" "Project_ZB_Cordinator.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "Project_ZB_Cordinator.axf" -A
	@echo ' '

	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Project_ZB_Cordinator.axf
	-@echo ' '

pre-build:
	-cmd //c 'C:\Users\asus\SIMPLI~1\V4_WOR~1\PROJEC~1/Project_ZB_Cordinator-prebuild.bat' 'Project_ZB_Cordinator' 'C:/Users/OS/Desktop/v4_workspace/v4_workspace/Project_ZB_Cordinator' '' 'C:\SiliconLabs\SimplicityStudio\v4'
	-@echo ' '

post-build:
	-cmd //c 'C:\Users\asus\SIMPLI~1\V4_WOR~1\PROJEC~1/Project_ZB_Cordinator-postbuild.bat' 'Project_ZB_Cordinator' 'C:/Users/OS/Desktop/v4_workspace/v4_workspace/Project_ZB_Cordinator' '' 'C:\SiliconLabs\SimplicityStudio\v4'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
