// kernel/cmos.h
#ifndef CMOS_H
#define CMOS_H

#include <stdint.h>

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} datetime_t;

// Инициализация
void cmos_init(void);

// Получение текущего времени
datetime_t cmos_get_datetime(void);

// Вывод времени на экран
void cmos_print_time(void);
void cmos_print_date(void);

#endif
