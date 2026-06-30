# STM32F103C8 循迹小车 - GCC Makefile

# Toolchain (ARM GNU Toolchain 12.2)
GCC_BIN   = /c/PROGRA~2/ARMGNU~1/12977F~1.2MP/bin
TOOLCHAIN = $(GCC_BIN)/arm-none-eabi-
CC      = $(TOOLCHAIN)gcc
CXX     = $(TOOLCHAIN)g++
AS      = $(TOOLCHAIN)gcc -x assembler-with-cpp
LD      = $(TOOLCHAIN)ld
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump
SIZE    = $(TOOLCHAIN)size

# Project files
TARGET  = firmware
BUILD_DIR = build
LINKER_SCRIPT = STM32F103C8.ld

# Source directories
SRC_DIRS = User Hardware Library Start delay

# Include paths
INC_DIRS = User Hardware Library Start delay

# Defines
DEFINES = -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000

# Compiler flags
CFLAGS  = -mcpu=cortex-m3 -mthumb
CFLAGS += -O2 -g -Wall -ffunction-sections -fdata-sections
CFLAGS += $(DEFINES)
CFLAGS += $(addprefix -I, $(INC_DIRS))
CFLAGS += -std=gnu99

# Linker flags
LDFLAGS  = -mcpu=cortex-m3 -mthumb
LDFLAGS += -specs=nano.specs -specs=nosys.specs
LDFLAGS += -T$(LINKER_SCRIPT)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref
LDFLAGS += -nostartfiles

# Collect source files
C_SRCS   = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
C_SRCS  += $(wildcard *.c)
ASM_SRCS = $(wildcard Start/*_gcc.S)

# Exclude Keil startup + old CMSIS core_cm3.c (GCC provides built-ins)
C_SRCS := $(filter-out Start/startup_stm32f10x_md.c Start/core_cm3.c, $(C_SRCS))

# Object files
C_OBJS   = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SRCS:.c=.o)))
ASM_OBJS = $(addprefix $(BUILD_DIR)/, $(notdir $(ASM_SRCS:.S=.o)))
OBJS     = $(C_OBJS) $(ASM_OBJS)

# VPATH for finding sources
VPATH = $(SRC_DIRS) .

.PHONY: all clean flash info

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin info

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Link
$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	@echo "  LD    $@"
	@$(CC) $(LDFLAGS) $(OBJS) -o $@

# Compile C
$(BUILD_DIR)/%.o: %.c
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Assemble
$(BUILD_DIR)/%.o: %.S
	@echo "  AS    $<"
	@$(AS) $(CFLAGS) -c $< -o $@

# Hex file
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	@echo "  HEX   $@"
	@$(OBJCOPY) -O ihex $< $@

# Binary file
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	@echo "  BIN   $@"
	@$(OBJCOPY) -O binary $< $@

# Size info
info: $(BUILD_DIR)/$(TARGET).elf
	@$(SIZE) $<

# Clean
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory"

# Flash via OpenOCD (optional)
flash: $(BUILD_DIR)/$(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $< 0x08000000 verify reset exit"
