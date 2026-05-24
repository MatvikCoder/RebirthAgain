CC = gcc
CFLAGS = -ffreestanding -nostdlib -static -m32
CFLAGS += -fno-stack-protector -fno-pie -no-pie
CFLAGS += -Wall -Wextra -I src/kernel

SRC_DIR = src/kernel
BUILD_DIR = build
TARGET = kernel.elf

C_SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/screen.c $(SRC_DIR)/keyboard.c
C_SRCS += $(SRC_DIR)/keymap.c $(SRC_DIR)/commands.c $(SRC_DIR)/cmos.c
C_SRCS += $(SRC_DIR)/panic.c

C_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRCS))

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(C_OBJS) $(SRC_DIR)/linker.ld
	$(CC) -m32 -T $(SRC_DIR)/linker.ld -o $@ $(C_OBJS) -nostdlib -static
	@echo "=== Build complete: $(TARGET) ==="

# Быстрый запуск через QEMU (прямая загрузка)
run: $(TARGET)
	qemu-system-x86_64 -kernel $(TARGET) -rtc base=localtime -display sdl -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0 -serial stdio -m 2G

# Создание ISO с GRUB
iso: $(TARGET)
	@echo "Creating ISO with GRUB..."
	@mkdir -p iso_root/boot/grub
	cp $(TARGET) iso_root/boot/
	cp grub.cfg iso_root/boot/grub/
	grub-mkrescue -o rebirthagain.iso iso_root/ 2>/dev/null || true
	@echo "ISO created: rebirthagain.iso"
# Запуск ISO в QEMU
run-iso: iso
	qemu-system-x86_64 -cdrom rebirthagain.iso -rtc base=localtime -display sdl -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0 -serial stdio -m 2G

# Создание ISO и запись на флешку (ОСТОРОЖНО!)
install-usb: iso
	@echo "WARNING: This will overwrite /dev/sdb!"
	@echo "Press Ctrl+C to cancel, Enter to continue..."
	@read -r
	sudo dd if=rebirthagain.iso of=/dev/sdb bs=4M status=progress
	sync
	@echo "Done! You can boot from USB now."

clean:
	rm -rf $(BUILD_DIR) $(TARGET) iso_root rebirthagain.iso
	@echo "Cleaned all"
