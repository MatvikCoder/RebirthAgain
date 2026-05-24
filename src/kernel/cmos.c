// kernel/cmos.c
#include "cmos.h"
#include "screen.h"

static uint8_t read_cmos(uint8_t reg) {
    __asm__ volatile ("outb %0, $0x70" : : "a"(reg));
    uint8_t result;
    __asm__ volatile ("inb $0x71, %0" : "=a"(result));
    return result;
}

static uint8_t bcd_to_bin(uint8_t bcd) {
    return (bcd & 0x0F) + ((bcd >> 4) * 10);
}

void cmos_init(void) {
    uint8_t status = read_cmos(0x0A);
    (void)status;
}

datetime_t cmos_get_datetime(void) {
    datetime_t dt;

    // Ждём, пока не обновится
    while (read_cmos(0x0A) & 0x80);

    dt.second = read_cmos(0x00);
    dt.minute = read_cmos(0x02);
    dt.hour   = read_cmos(0x04);
    dt.day    = read_cmos(0x07);
    dt.month  = read_cmos(0x08);
    dt.year   = read_cmos(0x09);

    // Конвертируем BCD в бинарный
    dt.second = bcd_to_bin(dt.second);
    dt.minute = bcd_to_bin(dt.minute);
    dt.hour   = bcd_to_bin(dt.hour);
    dt.day    = bcd_to_bin(dt.day);
    dt.month  = bcd_to_bin(dt.month);
    dt.year   = bcd_to_bin(dt.year);

    // Получаем статус регистр B (формат времени)
    uint8_t status_b = read_cmos(0x0B);
    int is_12hour = !(status_b & 0x02);  // бит 1 = 0 значит 12-часовой формат
    int is_pm = 0;

    // Если 12-часовой формат, конвертируем в 24-часовой
    if (is_12hour) {
        if (dt.hour & 0x80) {  // бит 7 = PM
            is_pm = 1;
            dt.hour &= 0x7F;    // убираем PM бит
        }

        if (is_pm && dt.hour != 12) {
            dt.hour += 12;
        } else if (!is_pm && dt.hour == 12) {
            dt.hour = 0;
        }
    }

    // Добавляем 2000 к году (если год < 70, то 2000+, иначе 1900+)
    if (dt.year < 70) {
        dt.year += 2000;
    } else {
        dt.year += 1900;
    }

    return dt;
}

void cmos_print_time(void) {
    datetime_t dt = cmos_get_datetime();

    if (dt.hour < 10) screen_putchar('0');
    print_dec(dt.hour);
    screen_putchar(':');

    if (dt.minute < 10) screen_putchar('0');
    print_dec(dt.minute);
    screen_putchar(':');

    if (dt.second < 10) screen_putchar('0');
    print_dec(dt.second);
}

void cmos_print_date(void) {
    datetime_t dt = cmos_get_datetime();

    if (dt.day < 10) screen_putchar('0');
    print_dec(dt.day);
    screen_putchar('/');

    if (dt.month < 10) screen_putchar('0');
    print_dec(dt.month);
    screen_putchar('/');

    print_dec(dt.year);
}
