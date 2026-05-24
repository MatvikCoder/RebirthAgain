#include <stdint.h>
#include "screen.h"

static uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void test_keyboard(void) {
    screen_clear();
    screen_print_color("Keyboard Test\n", LIGHT_CYAN, BLACK);
    screen_print_color("=============\n\n", YELLOW, BLACK);

    // Проверка контроллера клавиатуры
    uint8_t status = inb(0x64);
    screen_print("Status register (0x64): ");
    print_dec(status);
    screen_print(" (binary: ");
    for (int i = 7; i >= 0; i--) {
        if (status & (1 << i)) screen_print("1");
        else screen_print("0");
    }
    screen_print(")\n");

    // Проверка, есть ли данные
    if (status & 1) {
        screen_print("Data available!\n");
        uint8_t scancode = inb(0x60);
        screen_print("First scancode: ");
        print_dec(scancode);
        screen_print("\n");
    } else {
        screen_print("No data available.\n");
    }

    screen_print("\nPress any key to see scancode...\n");

    while (1) {
        if (inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);
            screen_print("Scancode: ");
            print_dec(scancode);
            screen_print(" (");
            if (scancode < 128) screen_print("PRESS");
            else screen_print("RELEASE");
            screen_print(")\n");
        }
        for (volatile int i = 0; i < 10000; i++);
    }
}
