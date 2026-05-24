// kernel/panic.h
#ifndef PANIC_H
#define PANIC_H

// Функция паники (красный экран смерти)
void kernel_panic(const char* message);

// Регистры CPU для отладки
void panic_with_regs(const char* message, unsigned int eip, unsigned int cs,
                      unsigned int eflags, unsigned int esp, unsigned int ebp);

#endif
