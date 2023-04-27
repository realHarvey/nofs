CROSS_COMPILE ?= arm-none-eabi-

AS  :=  $(CROSS_COMPILE)gcc -x assembler-with-cpp
CC  :=  $(CROSS_COMPILE)gcc
LD  :=  $(CROSS_COMPILE)ld
COPY := $(CROSS_COMPILE)objcopy
DUMP := $(CROSS_COMPILE)objdump
SIZE := $(CROSS_COMPILE)size

CPU = -mcpu=cortex-m3 \
	  -mthumb \
	  -mfloat-abi=soft
LIBS = -lm -lc -lnosys
INC =
DEPS = #-MMD -MP -MF"$(@:%.o=%.d)"

ASFLAGS = $(CPU) $(INC) $(DEPS) \
		-ffunction-sections -fdata-sections -fno-common -fmessage-length=0 \
		-Wall -Og -g
CFLAGS  = $(CPU) $(INC) $(DEPS) \
		-specs=nosys.specs \
		-ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -fno-builtin \
		-Og -g -std=gnu11 -Werror \
		-D MM=20
LDFLAGS = $(CPU) $(INC) \
		-specs=nano.specs -Tlinker.ld $(LIBS) \
		-Wl,--gc-sections,--print-memory-usage \
		-Wl,-Map=$(TARGET).map
