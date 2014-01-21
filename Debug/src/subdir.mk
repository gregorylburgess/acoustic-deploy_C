################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Bathy.cpp \
../src/Grid.cpp \
../src/acousticDeploy_C.cpp 

OBJS += \
./src/Bathy.o \
./src/Grid.o \
./src/acousticDeploy_C.o 

CPP_DEPS += \
./src/Bathy.d \
./src/Grid.d \
./src/acousticDeploy_C.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


