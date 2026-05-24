#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
int keyboard_hit(void);
char keyboard_getchar(void);

#endif
