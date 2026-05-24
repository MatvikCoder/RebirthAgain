#include "idt.h"
#include "screen.h"

struct idt_gate {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_gate idt[256];
static struct idt_ptr idt_ptr;

extern void keyboard_irq(void);

static void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

void idt_init(void) {
    screen_print_color("[INFO] Initializing IDT...\n", LIGHT_CYAN, BLACK);

    // Очищаем IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Устанавливаем обработчик клавиатуры (IRQ1 = 33)
    idt_set_gate(33, (uint32_t)keyboard_irq, 0x08, 0x8E);

    // Загружаем IDT
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)idt;
    __asm__ volatile ("lidt %0" : : "m" (idt_ptr));

    // Перепрограммируем PIC
    outb(0x20, 0x11);  // ICW1 master
    outb(0xA0, 0x11);  // ICW1 slave
    outb(0x21, 0x20);  // ICW2 master (IRQ0-7 -> INT 32-39)
    outb(0xA1, 0x28);  // ICW2 slave (IRQ8-15 -> INT 40-47)
    outb(0x21, 0x04);  // ICW3 master (slave on IRQ2)
    outb(0xA1, 0x02);  // ICW3 slave
    outb(0x21, 0x01);  // ICW4 master (8086 mode)
    outb(0xA1, 0x01);  // ICW4 slave
    outb(0x21, 0xFD);  // Маска: разрешаем только IRQ1 (клавиатура)
    outb(0xA1, 0xFF);  // Маска: запрещаем все slave IRQ

    // Включаем прерывания
    __asm__ volatile ("sti");

    screen_print_color("[OK] IDT initialized, interrupts enabled\n", LIGHT_GREEN, BLACK);
}
