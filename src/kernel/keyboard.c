#include <stdint.h>
#include "keyboard.h"
#include "screen.h"

extern char scancode_to_char(uint8_t scancode, int shift);

static int shift_pressed = 0;
static int caps_lock = 0;

static uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void keyboard_init(void) {
    screen_print_color("[OK] Keyboard initialized\n", LIGHT_GREEN, BLACK);
}

int keyboard_hit(void) {
    return inb(0x64) & 1;
}

char keyboard_getchar(void) {
    while (1) {
        if (keyboard_hit()) {
            uint8_t scancode = inb(0x60);

            // Обработка модификаторов
            if (scancode == 0x2A || scancode == 0x36) {
                shift_pressed = 1;
                continue;
            } else if (scancode == 0xAA || scancode == 0xB6) {
                shift_pressed = 0;
                continue;
            } else if (scancode == 0x3A) {
                caps_lock = !caps_lock;
                continue;
            }

            // Игнорируем отпускание клавиш
            if (scancode < 128) {
                int shift = shift_pressed || caps_lock;
                char c = scancode_to_char(scancode, shift);
                if (c) {
                    return c;
                }
            }
        }
    }
}
