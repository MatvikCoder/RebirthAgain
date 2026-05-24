#include <stdint.h>
#include "screen.h"

__attribute__((section(".multiboot")))
const unsigned int multiboot_header[] = {
    0x1BADB002, 0x00000003, -(0x1BADB002 + 0x00000003)
};

static uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void kernel_main(void) {
    screen_init();
    screen_clear();
    screen_print_color("=== Simple Keyboard Test ===\n", LIGHT_CYAN, BLACK);
    screen_print("Press any key to see scancode\n");
    screen_print("Press ESC to reboot\n\n");

    while (1) {
        // Проверяем клавиатуру
        if (inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);

            screen_print("Scancode: ");
            print_dec(scancode);
            screen_print(" (");

            if (scancode < 128) {
                screen_print("PRESS");
                if (scancode == 0x01) {
                    screen_print(" - ESC detected, rebooting...\n");
                    // reboot
                    __asm__ volatile (
                        "cli\n"
                        "movw $0x1234, 0x472\n"
                        "movb $0xFE, %%al\n"
                        "outb %%al, $0x64\n"
                        : : : "al"
                    );
                }
            } else {
                screen_print("RELEASE");
            }
            screen_print(")\n");
            screen_print_color("> ", LIGHT_CYAN, BLACK);
        }

        // Небольшая задержка
        for (volatile int i = 0; i < 1000; i++);
    }
}
