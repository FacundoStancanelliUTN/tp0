################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := server
BUILD_ARTIFACT_EXTENSION :=
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: server

dependents:
	-cd /home/utnso/tp0/libs2/Debug && $(MAKE) all

# Tool invocations
server: $(OBJS) $(USER_OBJS) /home/utnso/tp0/libs2/Debug/liblibs2.so makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/tp0/libs2/Debug" -o "server" $(OBJS) $(USER_OBJS) $(LIBS) -llibs2 -lcommons
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) server
	-@echo ' '

.PHONY: all clean dependents main-build
/home/utnso/tp0/libs2/Debug/liblibs2.so:

-include ../makefile.targets
