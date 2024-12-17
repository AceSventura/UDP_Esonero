################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Esonero\ client.c 

C_DEPS += \
./src/Esonero\ client.d 

OBJS += \
./src/Esonero\ client.o 


# Each subdirectory must supply rules for building sources it contributes
src/Esonero\ client.o: ../src/Esonero\ client.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Esonero client.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/Esonero\ client.d ./src/Esonero\ client.o

.PHONY: clean-src

