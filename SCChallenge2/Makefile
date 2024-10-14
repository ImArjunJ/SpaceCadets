# Directories
SRC_DIR := src
BUILD_DIR := build

# Source Files
ASM_SRC := $(SRC_DIR)/boot.asm
C_SRC := $(SRC_DIR)/kernel.c
INTERPRETER_SRC := $(SRC_DIR)/barebones.c
TERMINAL_SRC := $(SRC_DIR)/terminal.c
UTILS_SRC := $(SRC_DIR)/utils.c
MEM_SRC := $(SRC_DIR)/mem.c

# Object Files
ASM_OBJ := $(BUILD_DIR)/boot.o
C_OBJ := $(BUILD_DIR)/kernel.o
INTERPRETER_OBJ := $(BUILD_DIR)/barebones.o
TERMINAL_OBJ := $(BUILD_DIR)/terminal.o
UTILS_OBJ := $(BUILD_DIR)/utils.o
MEM_OBJ := $(BUILD_DIR)/mem.o

OUTPUT_BIN := kernel.bin

CFLAGS := -m32 -ffreestanding -fno-pic
LDFLAGS := -melf_i386 -T $(SRC_DIR)/linker.ld

DEBUG ?= 0  # Default to 0 (no debug mode)
ifeq ($(DEBUG), 1)
    CFLAGS += -DBBDEBUG=1
endif

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build targets
all: $(OUTPUT_BIN)

$(OUTPUT_BIN): $(ASM_OBJ) $(C_OBJ) $(INTERPRETER_OBJ) $(TERMINAL_OBJ) $(UTILS_OBJ) $(MEM_OBJ)
	ld $(LDFLAGS) -o $(OUTPUT_BIN) $(ASM_OBJ) $(C_OBJ) $(INTERPRETER_OBJ) $(TERMINAL_OBJ) $(UTILS_OBJ) $(MEM_OBJ)

$(ASM_OBJ): $(ASM_SRC) | $(BUILD_DIR)
	nasm -f elf32 $(ASM_SRC) -o $(ASM_OBJ)

$(C_OBJ): $(C_SRC) | $(BUILD_DIR)
	gcc $(CFLAGS) -c $(C_SRC) -o $(C_OBJ)

$(INTERPRETER_OBJ): $(INTERPRETER_SRC) | $(BUILD_DIR)
	gcc $(CFLAGS) -c $(INTERPRETER_SRC) -o $(INTERPRETER_OBJ)

$(TERMINAL_OBJ): $(TERMINAL_SRC) | $(BUILD_DIR)
	gcc $(CFLAGS) -c $(TERMINAL_SRC) -o $(TERMINAL_OBJ)

$(UTILS_OBJ): $(UTILS_SRC) | $(BUILD_DIR)
	gcc $(CFLAGS) -c $(UTILS_SRC) -o $(UTILS_OBJ)

$(MEM_OBJ): $(MEM_SRC) | $(BUILD_DIR)
	gcc $(CFLAGS) -c $(MEM_SRC) -o $(MEM_OBJ)

# Clean up build files
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_BIN)

# Run QEMU with the built kernel binary
run: all
	qemu-system-i386 -kernel $(OUTPUT_BIN)

.PHONY: all clean run run-debug
