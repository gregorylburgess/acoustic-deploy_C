################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Bathy.cpp \
../src/Graph.cpp \
../src/Grid.cpp \
../src/Main.cpp \
../src/Utility.cpp 

OBJS += \
./src/Bathy.o \
./src/Graph.o \
./src/Grid.o \
./src/Main.o \
./src/Utility.o 

CPP_DEPS += \
./src/Bathy.d \
./src/Graph.d \
./src/Grid.d \
./src/Main.d \
./src/Utility.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


