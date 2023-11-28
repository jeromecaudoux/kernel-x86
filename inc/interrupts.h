#ifndef INTERRUPTS_H_
# define INTERRUPTS_H_

# include "types.h"

# define IDT_SIZE			256

# define INTERRUPT_GATE_FLAGS_BASE	0x06

# define SEG_PRESENT		1 << 7
# define SEG_NOT_PRESENT	0 << 7

# define _RING_LVL_0		(0 << 5 && 0 << 6)
# define RING_LVL_1		1 << 5
# define RING_LVL_2		2 << 5
# define RING_LVL_3		3 << 5

# define ISR_USER		RING_LVL_3

# define SIZE_OF_GATE_16	0x06
# define SIZE_OF_GATE_32	0x0E

struct __attribute__((__packed__)) idt_flags_fields {
	uint8_t		gate : 5;
	uint8_t		dpl : 2;
	uint8_t		p : 1;
};

union idt_flags {
	uint8_t			flags;
	struct idt_flags_fields	fields;
};

// esi edi ebp -eip- esp

typedef struct __attribute__((packed)) regs
{
	/* uint32_t ds;                  // Data segment selector */
	uint32_t edi;                  // Data segment selector
	uint32_t esi;                  // Data segment selector
	uint32_t ebp;                  // Data segment selector
	uint32_t esp;                  // Data segment selector
	uint32_t ebx;                  // Data segment selector
	uint32_t edx;                  // Data segment selector
	uint32_t ecx;                  // Data segment selector
	uint32_t eax;		       // Pushed by pusha.

	uint32_t ss;
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;

	uint32_t int_no;                  // Data segment selector
	uint32_t err_code;    // Interrupt number and error code (if applicable)
	uint32_t eip;                  // Data segment selector
	uint16_t cs;                  // Data segment selector
	uint32_t eflags;                  // Data segment selector
	uint32_t useresp;                  // Data segment selector
	uint32_t _ss; // Pushed by the processor automatically.
} registers_t;

struct __attribute__((__packed__)) idt_entry {
	uint16_t		offset_low : 16;
	uint16_t		sgm_selector : 16;
	uint8_t			zero : 8;
	union idt_flags		flags;
	uint16_t		offset_high : 16;
};

struct __attribute__((__packed__)) idt_register {
	uint16_t	limit;
	uint32_t	base;
};

typedef struct idt_entry idt_entry_t;
typedef struct idt_register idt_register_t;
typedef void (*isr_handler)(void);
typedef void (*interrupt_handler)(struct regs* regs);

void register_irq_handler(int irq_number, interrupt_handler handler);

void irq_handler(struct regs* regs);

void interrupts_init(void);
void interrupts_set_isr(int n, isr_handler handler, int flags);

extern void	isr0();
extern void	isr1();
extern void	isr2();
extern void	isr3();
extern void	isr4();
extern void	isr5();
extern void	isr6();
extern void	isr7();
extern void	isr8();
extern void	isr9();
extern void	isr10();
extern void	isr11();
extern void	isr12();
extern void	isr13();
extern void	isr14();

extern void	isr16();
extern void	isr17();
extern void	isr18();
extern void	isr19();
extern void	isr20();

extern void	isr30();
extern void	isr31();

extern void	isr32(); // timer
extern void	isr33(); // keyboard
extern void	isr36(); // uart com1 and com3


#endif /* !INTERRUPTS_H_ */
