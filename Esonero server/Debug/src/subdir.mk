################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Esonero\ server.c 

C_DEPS += \
./src/Esonero\ server.d 

OBJS += \
./src/Esonero\ server.o 


# Each subdirectory must supply rules for building sources it contributes
src/Esonero\ server.o: ../src/Esonero\ server.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Esonero server.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/Esonero\ server.d ./src/Esonero\ server.o

.PHONY: clean-src

