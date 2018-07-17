################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/UartFun.c \
../src/cJSON.c \
../src/circlebuff.c \
../src/monitor.c \
../src/mqtt-pub.c \
../src/msgDispatcher.c \
../src/rfcomm_server.c \
../src/sample.c \
../src/spiFuntions.c \
../src/sqlite3_task.c 

OBJS += \
./src/UartFun.o \
./src/cJSON.o \
./src/circlebuff.o \
./src/monitor.o \
./src/mqtt-pub.o \
./src/msgDispatcher.o \
./src/rfcomm_server.o \
./src/sample.o \
./src/spiFuntions.o \
./src/sqlite3_task.o 

C_DEPS += \
./src/UartFun.d \
./src/cJSON.d \
./src/circlebuff.d \
./src/monitor.d \
./src/mqtt-pub.d \
./src/msgDispatcher.d \
./src/rfcomm_server.d \
./src/sample.d \
./src/spiFuntions.d \
./src/sqlite3_task.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"/home/ryan/eclipse-workspace/jx-linux-wg01-v01/lib/arm-lib/mqtt/mqtt-h" -I"/home/ryan/eclipse-workspace/jx-linux-wg01-v01/lib/arm-lib/bluetooth/lib" -I"/home/ryan/eclipse-workspace/jx-linux-wg01-v01/lib/arm-lib/bluetooth/include" -I"/home/ryan/eclipse-workspace/jx-linux-wg01-v01/lib/arm-lib/sqlite3" -I/home/ryan/arm-linux-gcc/arm-linux-gnueabihf/libc/lib -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


