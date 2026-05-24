#include <stdint.h>
#include "screen.h"
#include "keyboard.h"
#include "commands.h"
#include "cmos.h"

__attribute__((section(".multiboot")))
const unsigned int multiboot_header[] = {
    0x1BADB002, 0x00000003, -(0x1BADB002 + 0x00000003)
};

void system_reboot(void) {
    __asm__ volatile (
        "cli\n"
        "movw $0x1234, 0x472\n"
        "movb $0xFE, %%al\n"
        "outb %%al, $0x64\n"
        : : : "al"
    );
    while(1) __asm__("hlt");
}

void system_shutdown(void) {
    while(1) __asm__("hlt");
}

static char input_buffer[256];
static int input_pos = 0;

static void process_input(void) {
    input_buffer[input_pos] = '\0';
    screen_putchar('\n');
    commands_execute(input_buffer);
    screen_print_color("> ", LIGHT_CYAN, BLACK);
    input_pos = 0;
}

void kernel_main(void) {
    screen_init();
    cmos_init();
    keyboard_init();
    commands_init();

    screen_clear();
    screen_print_color("RebirthAgain OS v1.0\n", LIGHT_CYAN, BLACK);
    screen_print_color("========================================\n", YELLOW, BLACK);

    print_info();
    screen_print("Kernel loaded\n");

    print_ok();
    screen_print("Screen initialized\n");

    print_ok();
    screen_print("Keyboard ready\n");

    print_ok();
    screen_print("CMOS RTC initialized\n");

    screen_print("\n");
    screen_print_color("> ", LIGHT_CYAN, BLACK);

    while (1) {
        if (keyboard_hit()) {
            char c = keyboard_getchar();

            if (c == '\n') {
                if (input_pos > 0) {
                    process_input();
                } else {
                    screen_putchar('\n');
                    screen_print_color("> ", LIGHT_CYAN, BLACK);
                }
            } else if (c == '\b') {
                if (input_pos > 0) {
                    input_pos--;
                    screen_putchar('\b');
                }
            } else if (c >= 32 && c <= 126) {
                if (input_pos < 255) {
                    input_buffer[input_pos++] = c;
                    screen_putchar(c);
                }
            }
        }
    }
}
