#include "screen.h"
#include "keyboard.h"

static volatile char last_key = 0;
static volatile int key_ready = 0;

void keyboard_irq_handler(void) {
    uint8_t scancode;
    __asm__ volatile ("inb $0x60, %0" : "=a"(scancode));

    // Передаём в keyboard.c
    extern void process_scancode(uint8_t scancode);
    process_scancode(scancode);

    // Отправляем EOI
    __asm__ volatile (
        "mov $0x20, %al\n"
        "out %al, $0x20\n"
    );
}

// Функции для keyboard.c
int isr_key_ready(void) {
    return key_ready;
}

char isr_get_key(void) {
    key_ready = 0;
    return last_key;
}

void isr_set_key(char c) {
    last_key = c;
    key_ready = 1;
}
