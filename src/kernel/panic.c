#include "panic.h"
#include "screen.h"

// Функция для звукового сигнала (beep)
static void beep(void) {
    // Включаем PIT канал 2 для динамика
    __asm__ volatile (
        "movb $0xB6, %%al\n"
        "outb %%al, $0x43\n"
        "movw $1350, %%ax\n"     // 1193180 / 880 ≈ 1356 (частота ~880 Гц)
    "outb %%al, $0x42\n"
    "movb %%ah, %%al\n"
    "outb %%al, $0x42\n"
    "inb $0x61, %%al\n"
    "orb $3, %%al\n"
    "outb %%al, $0x61\n"
    :
    :
    : "al", "ax"
    );

    // Ждём
    for (volatile int i = 0; i < 500000; i++);

    // Выключаем звук
    __asm__ volatile (
        "inb $0x61, %%al\n"
        "andb $0xFC, %%al\n"
        "outb %%al, $0x61\n"
        :
        :
        : "al"
    );
}

// Функция для сигнала "SOS"
static void beep_sos(void) {
    // Три коротких
    for (int i = 0; i < 3; i++) {
        beep();
        for (volatile int j = 0; j < 200000; j++);
    }

    // Пауза
    for (volatile int j = 0; j < 500000; j++);

    // Три длинных
    for (int i = 0; i < 3; i++) {
        beep();
        for (volatile int j = 0; j < 500000; j++);
    }

    // Пауза
    for (volatile int j = 0; j < 500000; j++);

    // Три коротких
    for (int i = 0; i < 3; i++) {
        beep();
        for (volatile int j = 0; j < 200000; j++);
    }
}

void kernel_panic(const char* message) {
    // Проигрываем сигнал SOS
    beep_sos();

    char* video = (char*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        video[i*2] = ' ';
        video[i*2+1] = (RED << 4) | WHITE;
    }

    int x = 0, y = 0;

    void panic_putchar(char c) {
        if (c == '\n') {
            x = 0;
            y++;
        } else {
            int pos = (y * 80 + x) * 2;
            video[pos] = c;
            video[pos+1] = (RED << 4) | WHITE;
            x++;
            if (x >= 80) {
                x = 0;
                y++;
            }
        }
    }

    void panic_print(const char* str) {
        while (*str) panic_putchar(*str++);
    }

    for (int i = 0; i < 80; i++) panic_putchar('=');
    panic_putchar('\n');

    panic_print("                                 KERNEL PANIC                         \n");

    panic_putchar('\n');

    for (int i = 0; i < 80; i++) panic_putchar('=');
    panic_putchar('\n');
    panic_putchar('\n');

    panic_print(message);
    panic_putchar('\n');
    panic_putchar('\n');

    panic_print("System halted.\n");
    panic_putchar('\n');

    while(1) __asm__("hlt");
}
