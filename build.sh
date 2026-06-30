#!/bin/bash
# STM32F103C8 循迹小车 - GCC Build Script
set -e

GCC_BIN="/c/PROGRA~2/ARMGNU~1/12977F~1.2MP/bin"
CC="$GCC_BIN/arm-none-eabi-gcc"
OBJCOPY="$GCC_BIN/arm-none-eabi-objcopy"
SIZE="$GCC_BIN/arm-none-eabi-size"

BUILD_DIR="build"
TARGET="firmware"
LINKER_SCRIPT="STM32F103C8.ld"

INC_DIRS="-IUser -IHardware -ILibrary -IStart -Idelay"
DEFINES="-DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000"
CFLAGS="-mcpu=cortex-m3 -mthumb -O2 -g -Wall -ffunction-sections -fdata-sections $DEFINES $INC_DIRS -std=gnu99"
LDFLAGS="-mcpu=cortex-m3 -mthumb -specs=nano.specs -specs=nosys.specs -T$LINKER_SCRIPT -Wl,--gc-sections -Wl,-Map=$BUILD_DIR/$TARGET.map,--cref -nostartfiles"

mkdir -p "$BUILD_DIR"

echo "=== Compiling ==="

# Collect all C sources (excluding Keil startup + old CMSIS core_cm3.c)
SOURCES_C=$(find User Hardware Library Start delay -name "*.c" \
    -not -name "startup_stm32f10x_md.c" \
    -not -name "core_cm3.c" 2>/dev/null)
SOURCES_CXX=$(find User Hardware Library Start delay -name "*.C" 2>/dev/null)
SOURCES="$SOURCES_C $SOURCES_CXX"

for src in $SOURCES; do
    obj="$BUILD_DIR/$(basename "$src" .c).o"
    echo "  CC    $src"
    "$CC" $CFLAGS -c "$src" -o "$obj"
done

# Assemble GCC startup file
echo "  AS    Start/startup_stm32f10x_md_gcc.S"
"$CC" $CFLAGS -c "Start/startup_stm32f10x_md_gcc.S" -o "$BUILD_DIR/startup_stm32f10x_md_gcc.o"

echo ""
echo "=== Linking ==="
OBJS=$(find "$BUILD_DIR" -name "*.o")
"$CC" $LDFLAGS $OBJS -o "$BUILD_DIR/$TARGET.elf"

echo "=== Generating HEX and BIN ==="
"$OBJCOPY" -O ihex "$BUILD_DIR/$TARGET.elf" "$BUILD_DIR/$TARGET.hex"
"$OBJCOPY" -O binary "$BUILD_DIR/$TARGET.elf" "$BUILD_DIR/$TARGET.bin"

echo ""
echo "=== Build Complete ==="
"$SIZE" "$BUILD_DIR/$TARGET.elf"
