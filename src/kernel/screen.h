#ifndef SCREEN_H
#define SCREEN_H

enum colors {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GRAY = 7,
    DARK_GRAY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};

void screen_init(void);
void screen_clear(void);
void screen_set_color(int fg, int bg);
void screen_putchar(char c);
void screen_print(const char* s);
void screen_print_color(const char* s, int fg, int bg);
void screen_gotoxy(int x, int y);
int screen_get_x(void);
int screen_get_y(void);
void print_dec(int num);
void print_ok(void);
void print_fail(void);
void print_info(void);
void print_warn(void);
void test_keyboard(void);

extern int current_fg;
extern int current_bg;

#endif
