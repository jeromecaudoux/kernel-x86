#include "interrupts.h"
#include "vga.h"
#include "main.h"
#include "string.h"
#include "serial_port.h"
#include "irq.h"
#include "tools.h"
#include "pit.h"
#include "io.h"
#include "kb.h"
#include "uart.h"
#include "kprintf.h"
#include "registers.h"

static idt_entry_t		g_idt[IDT_SIZE];
static interrupt_handler	g_handlers[256];


char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Fault",
    "Machine Check", 
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

static void load_idt_register(void) {
	idt_register_t idt_register;

	idt_register.base = (uint32_t)&g_idt;
	idt_register.limit = (sizeof(idt_entry_t) * IDT_SIZE) - 1;

	__asm__ volatile("lidt %0\n"
			 : /* no output */
			 : "m" (idt_register)
			 : "memory");
}

void register_irq_handler(int int_number, interrupt_handler handler)
{
	g_handlers[int_number] = handler;
}

// https://wiki.osdev.org/Exceptions#General_Protection_Fault
void irq_handler(struct regs* regs)
{
	if (g_handlers[regs->int_no] != NULL) {
		(g_handlers[regs->int_no])(regs);
	}

	if (regs->int_no >= 32 && regs->int_no < 48) {
		irq_send_eoi(regs->int_no);
	} else if (regs->int_no < 32) {
		kprintf("Unhandled interrupt: %s\n", exception_messages[regs->int_no]);
		kprintf("Error code: %d b%b\n", regs->err_code, regs->err_code);
		print_cr0();
		kprintf("int_no: %d\n", regs->int_no);
		kprintf("EIP: 0x%x\n", regs->eip);
		kprintf("CS: 0x%x\n", regs->cs);
		kprintf("EFLAGS: 0x%x\n", regs->eflags);
		kprintf("ESP: 0x%x\n", regs->useresp);
		kprintf("SS: %x\n", regs->ss);
		kprintf("DS: 0x%x\n", regs->ds);
		kprintf("ES: 0x%x\n", regs->es);
		kprintf("FS: 0x%x\n", regs->fs);
		kprintf("GS: 0x%x\n", regs->gs);
		kprintf("EDI: 0x%x\n", regs->edi);
		kprintf("ESI: 0x%x\n", regs->esi);
		kprintf("EBP: 0x%x\n", regs->ebp);
		kprintf("ESP: 0x%x\n", regs->esp);
		kprintf("EBX: 0x%x\n", regs->ebx);
		kprintf("EDX: 0x%x\n", regs->edx);
		kprintf("ECX: 0x%x\n", regs->ecx);
		kprintf("EAX: 0x%x\n", regs->eax);
		kprintf("Stoping the kernel\n");
		while (1);
	}
}

// static void disable_interrupts(void)
// {
// 	asm("cli");
// }

static void enable_interrupts(void)
{
	asm("sti");
}

void interrupts_init(void)
{
	uint8_t flags;

	memset(g_idt, 0, sizeof(idt_entry_t) * IDT_SIZE);

	flags = SEG_PRESENT | _RING_LVL_0 | SIZE_OF_GATE_32;
	interrupts_set_isr(0, &isr0, flags);
	interrupts_set_isr(1, &isr1, flags);
	interrupts_set_isr(2, &isr2, flags);
	interrupts_set_isr(3, &isr3, flags);
	interrupts_set_isr(4, &isr4, flags);
	interrupts_set_isr(5, &isr5, flags);
	interrupts_set_isr(6, &isr6, flags);
	interrupts_set_isr(7, &isr7, flags);
	interrupts_set_isr(8, &isr8, flags);
	interrupts_set_isr(9, &isr9, flags);
	interrupts_set_isr(10, &isr10, flags);
	interrupts_set_isr(11, &isr11, flags);
	interrupts_set_isr(12, &isr12, flags);
	interrupts_set_isr(13, &isr13, flags);
	interrupts_set_isr(14, &isr14, flags);

	interrupts_set_isr(16, &isr16, flags);
	interrupts_set_isr(17, &isr17, flags);
	interrupts_set_isr(18, &isr18, flags);
	interrupts_set_isr(19, &isr19, flags);
	interrupts_set_isr(20, &isr20, flags);

	interrupts_set_isr(30, &isr30, flags);
	interrupts_set_isr(31, &isr31, flags);

	interrupts_set_isr(32, &isr32, flags); // Programmable Interrupt Timer Interrupt
	interrupts_set_isr(33, &isr33, flags); // Keyboard Interrupt
	interrupts_set_isr(36, &isr36, flags); // UART com1 and com3

	load_idt_register();
	enable_interrupts();
}

void interrupts_set_isr(int n, isr_handler handler, int flags)
{
	if (n >= IDT_SIZE)
		return ;
	g_idt[n].offset_low = ((uint32_t)handler & 0x0000FFFF);
	g_idt[n].sgm_selector = 0x08;
	g_idt[n].zero = 0x00;
	g_idt[n].flags.flags = flags;
	g_idt[n].offset_high = ((uint32_t)handler & 0xFFFF0000) >> 16;
}
