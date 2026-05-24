#include "screen.h"

static int x = 0, y = 0;
int current_fg = LIGHT_GRAY;
int current_bg = BLACK;

static void outb(unsigned short port, unsigned char data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "d"(port));
}

static void update_cursor(void) {
    unsigned short pos = y * 80 + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, pos & 0xFF);
    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8) & 0xFF);
}

void screen_init(void) {
    screen_clear();
}

void screen_clear(void) {
    char* v = (char*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        v[i*2] = ' ';
        v[i*2+1] = (BLACK << 4) | LIGHT_GRAY;
    }
    x = y = 0;
    update_cursor();
}

void screen_set_color(int fg, int bg) {
    current_fg = fg;
    current_bg = bg;
}

void screen_putchar(char c) {
    char* v = (char*)0xB8000;

    if (c == '\n') {
        x = 0;
        y++;
    } else if (c == '\b') {
        if (x > 0) {
            x--;
            v[(y*80+x)*2] = ' ';
        }
    } else {
        v[(y*80+x)*2] = c;
        v[(y*80+x)*2+1] = (current_bg << 4) | current_fg;
        x++;
        if (x >= 80) {
            x = 0;
            y++;
        }
    }

    if (y >= 25) {
        for (int i = 1; i < 25; i++) {
            for (int j = 0; j < 80; j++) {
                v[((i-1)*80+j)*2] = v[(i*80+j)*2];
                v[((i-1)*80+j)*2+1] = v[(i*80+j)*2+1];
            }
        }
        for (int j = 0; j < 80; j++) {
            v[(24*80+j)*2] = ' ';
            v[(24*80+j)*2+1] = (current_bg << 4) | current_fg;
        }
        y = 24;
    }

    update_cursor();
}

void screen_print(const char* s) {
    while (*s) screen_putchar(*s++);
}

void screen_print_color(const char* s, int fg, int bg) {
    int old_fg = current_fg, old_bg = current_bg;
    screen_set_color(fg, bg);
    screen_print(s);
    screen_set_color(old_fg, old_bg);
}

void screen_gotoxy(int new_x, int new_y) {
    if (new_x >= 0 && new_x < 80 && new_y >= 0 && new_y < 25) {
        x = new_x;
        y = new_y;
        update_cursor();
    }
}

int screen_get_x(void) { return x; }
int screen_get_y(void) { return y; }

void print_dec(int num) {
    if (num == 0) {
        screen_putchar('0');
        return;
    }
    if (num < 0) {
        screen_putchar('-');
        num = -num;
    }
    char digits[10];
    int i = 0;
    while (num > 0) {
        digits[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i > 0) screen_putchar(digits[--i]);
}

void print_ok(void) { screen_print_color("[OK] ", LIGHT_GREEN, BLACK); }
void print_fail(void) { screen_print_color("[FAIL] ", LIGHT_RED, BLACK); }
void print_info(void) { screen_print_color("[INFO] ", LIGHT_CYAN, BLACK); }
void print_warn(void) { screen_print_color("[WARN] ", YELLOW, BLACK); }
