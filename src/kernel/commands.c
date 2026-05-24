#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "commands.h"
#include "screen.h"
#include "cmos.h"
#include "panic.h"

// Внешние функции из main.c
extern void system_reboot(void);
extern void system_shutdown(void);

static void cmd_help(const char* args) {
    (void)args;
    screen_print("Available commands:\n");
    screen_print("  help     -  Show this help\n");
    screen_print("  clear    -  Clear screen\n");
    screen_print("  echo     -  Print text\n");
    screen_print("  time     -  Show current time\n");
    screen_print("  date     -  Show current date\n");
    screen_print("  about    -  Show system info\n");
    screen_print("  panic    -  Trigger kernel panic\n");
    screen_print("  reboot   -  Reboot system\n");
    screen_print("  shutdown -  Shutdown (QEMU only)\n");
    screen_print("  sysfetch -  Show about (from about command) and logo\n");
}

static uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static void cmd_clear(const char* args) {
    (void)args;
    screen_clear();
}

static void cmd_echo(const char* args) {
    if (args) screen_print(args);
    screen_putchar('\n');
}

static void cmd_logo(const char* args) {
    (void)args;
    screen_print_color("\n", LIGHT_CYAN, BLACK);
    screen_print_color("    ___     _    _     _   _        _             _      \n", LIGHT_CYAN, BLACK);
    screen_print_color("   | _ \\___| |__(_)_ _| |_| |_     /_\\  __ _ __ _(_)_ _  \n", LIGHT_CYAN, BLACK);
    screen_print_color("   |   / -_) '_ \\ | '_|  _| ' \\   / _ \\/ _` / _` | | ' \\ \n", LIGHT_CYAN, BLACK);
    screen_print_color("   |_|_\\___|_.__/_|_|  \\__|_||_|_/_/ \\_\\__, \\__,_|_|_||_|\n", LIGHT_CYAN, BLACK);
    screen_print_color("                                |___|   |___/            \n", LIGHT_CYAN, BLACK);
    screen_print_color("\n", LIGHT_CYAN, BLACK);
    screen_print_color("   RebirthAgain OS - Dayte mne moy magniy B6\n", LIGHT_GREEN, BLACK);
    screen_print_color("\n", LIGHT_CYAN, BLACK);

    screen_print_color("\n", LIGHT_CYAN, BLACK);
    screen_print_color("========================================\n", YELLOW, BLACK);
    screen_print("A simple 32-bit kernel with:\n");
    screen_print("  - GRUB Multiboot loading\n");
    screen_print("  - Color text output\n");
    screen_print("  - Keyboard interrupt handling (IDT)\n");
    screen_print("  - CMOS time/date reading\n");
    screen_print("  - Simple command shell\n");
    screen_print("\n");
}

static void cmd_time(const char* args) {
    (void)args;
    cmos_print_time();
    screen_putchar('\n');
}

static void cmd_date(const char* args) {
    (void)args;
    cmos_print_date();
    screen_putchar('\n');
}

static void cmd_kbdtest(const char* args) {
    (void)args;
    screen_print("Testing...\n");
    uint8_t scancode = inb(0x60);
    print_dec(scancode);
    screen_print("\n");
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
    }

}

static void cmd_about(const char* args) {
    (void)args;
    screen_print_color("RebirthAgain OS v1.0\n", LIGHT_CYAN, BLACK);
    screen_print_color("========================================\n", YELLOW, BLACK);
    screen_print("A simple 32-bit kernel with:\n");
    screen_print("  - GRUB Multiboot loading\n");
    screen_print("  - Color text output\n");
    screen_print("  - Keyboard interrupt handling (IDT)\n");
    screen_print("  - CMOS time/date reading\n");
    screen_print("  - Simple command shell\n");
    screen_print("\n");
    screen_print_color("License: GNU General Public License v3.0\n", LIGHT_GREEN, BLACK);
    screen_print("This is free software; see the source for copying conditions.\n");
    screen_print("There is NO warranty; not even for MERCHANTABILITY or FITNESS\n");
    screen_print("FOR A PARTICULAR PURPOSE.\n");
}

static void cmd_panic(const char* args) {
    (void)args;
    kernel_panic("An error has been occured. Please, restart your PC.");
}

static void cmd_reboot(const char* args) {
    (void)args;
    screen_print("Rebooting...\n");
    system_reboot();
}

static void cmd_shutdown(const char* args) {
    (void)args;
    screen_print("Shutting down...\n");
    system_shutdown();
}

static const struct {
    const char* name;
    void (*handler)(const char*);
} commands[] = {
    {"help", cmd_help},
    {"clear", cmd_clear},
    {"echo", cmd_echo},
    {"time", cmd_time},
    {"date", cmd_date},
    {"about", cmd_about},
    {"panic", cmd_panic},
    {"reboot", cmd_reboot},
    {"shutdown", cmd_shutdown},
    {"kbdkbd", cmd_kbdtest},
    {"sysfetch",  cmd_logo},
    {NULL, NULL}
};

void commands_init(void) {}

void commands_execute(const char* cmdline) {
    if (!cmdline || cmdline[0] == '\0') return;

    char cmd[32];
    int i = 0;
    while (cmdline[i] && cmdline[i] != ' ' && i < 31) {
        cmd[i] = cmdline[i];
        i++;
    }
    cmd[i] = '\0';

    const char* args = cmdline[i] == ' ' ? cmdline + i + 1 : NULL;

    for (int j = 0; commands[j].name; j++) {
        const char* a = cmd;
        const char* b = commands[j].name;
        int match = 1;
        while (*a && *b) {
            if (*a != *b) {
                match = 0;
                break;
            }
            a++; b++;
        }
        if (match && *a == '\0' && *b == '\0') {
            commands[j].handler(args);
            return;
        }
    }
    screen_print("Unknown command. Type 'help'.\n");
}
