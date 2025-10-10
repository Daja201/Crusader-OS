BUILD_DIR ?= build
ASM ?= nasm
CC ?= gcc
LD ?= ld

.PHONY: all bootloader kernel floppy_image clean

all: floppy_image

#
# Bootloader
#
bootloader: stage1 stage2

stage1:
	@$(MAKE) -C src/bootloader/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2:
	@$(MAKE) -C src/bootloader/stage2 BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Kernel
#
kernel:
	@$(MAKE) -C src/kernel BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Floppy image
#
floppy_image: bootloader kernel
	@mkdir -p $(BUILD_DIR)
	@dd if=/dev/zero of=$(BUILD_DIR)/myos.img bs=512 count=32768 >/dev/null
	@mkfs.fat -F 16 -n "NBOS" $(BUILD_DIR)/myos.img >/dev/null
	@dd if=$(BUILD_DIR)/stage1.bin of=$(BUILD_DIR)/myos.img conv=notrunc >/dev/null
	@mcopy -i $(BUILD_DIR)/myos.img $(BUILD_DIR)/stage2.bin "::STAGE2.BIN"
	@mcopy -i $(BUILD_DIR)/myos.img $(BUILD_DIR)/kernel.bin "::KERNEL.BIN"
	@echo "--> Created floppy image: $(BUILD_DIR)/myos.img"

#
# Clean
#
clean:
	@$(MAKE) -C src/bootloader/stage1 clean
	@$(MAKE) -C src/bootloader/stage2 clean
	@$(MAKE) -C src/kernel clean
	@rm -rf $(BUILD_DIR)/*
