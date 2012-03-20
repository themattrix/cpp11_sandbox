################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/util/pending_queue.cpp \
../src/util/string.cpp 

OBJS += \
./src/util/pending_queue.o \
./src/util/string.o 

CPP_DEPS += \
./src/util/pending_queue.d \
./src/util/string.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/%.o: ../src/util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	/opt/local/bin/g++-mp-4.7 -D__GXX_EXPERIMENTAL_CXX0X__ -I/opt/local/include/gcc47/c++ -O3 -Wall -Wextra -Werror -Wconversion -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


