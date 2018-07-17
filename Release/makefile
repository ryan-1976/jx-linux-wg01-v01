################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
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
all: jx-linux-wg01-v01

# Tool invocations
jx-linux-wg01-v01: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	arm-linux-gnueabihf-gcc -L"/home/ryan/eclipse-workspace/jx-linux-wg01-v01/lib/arm-lib/mqtt" -L"/home/ryan/eclipse-workspace/jx-linux-wg01-v01/lib/arm-lib/bluetooth/lib" -L"/home/ryan/eclipse-workspace/jx-linux-wg01-v01/lib/arm-lib/sqlite3" -o "jx-linux-wg01-v01" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) jx-linux-wg01-v01
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
