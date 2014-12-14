################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fs/src/file_sys.c \
../fs/src/sha1.c 

OBJS += \
./fs/src/file_sys.o \
./fs/src/sha1.o 

C_DEPS += \
./fs/src/file_sys.d \
./fs/src/sha1.d 


# Each subdirectory must supply rules for building sources it contributes
fs/src/%.o: ../fs/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/rishoo/workspace/dedup4/fs/inc" -I"/home/rishoo/workspace/dedup4/inc" -I"/home/rishoo/workspace/dedup4/cac" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


