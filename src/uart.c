#include "io.h"
#include "main.h"
#include "tools.h"
#include "uart.h"
#include "irq.h"
#include "interrupts.h"
#include "serial_port.h"

static uint16_t uart_addr = 0;

void		uart_handler(__attribute__((unused))struct regs *regs)
{
	printk("UART : ");
	char c = uart_recv();
	uart_putchar(c);
	uart_putchar('\n');
}

void		uart_init(const uint16_t addr)
{

	//if (addr != COM1_ADDR && addr != COM2_ADDR && addr != COM3_ADDR && addr != COM4_ADDR) {
	if (addr != SERIAL_PORT) {
		printk(ERR_UART_BADADDR);
		return ;
	}

	if (addr == uart_addr) {
		printk(ERR_UART_ALREADYINIT);
		return ;
	}

	uart_addr = addr;
	outb(uart_addr + 1, 0x01);
	outb(uart_addr + 2, 0x00);
	outb(uart_addr + 3, 0x80);
	outb(uart_addr + 0, 0x03);
	outb(uart_addr + 1, 0x00);
	outb(uart_addr + 3, 0x03);
	outb(uart_addr + 4, 0x0B);

	irq_unmask(4);
	register_irq_handler(36, uart_handler);
}

bool		uart_is_init(void) {
	return ((uart_addr != 0) ? (TRUE) : (FALSE));
}

void		uart_putchar(const uint8_t c) {
	uart_send(c);
}

void		uart_putstr(const char *msg) {
	size_t		i;
	size_t		size;

	i = 0;
	size = strlen(msg);
	while (i < size) {
		uart_putchar(msg[i]);
		++i;
	}
}

void		uart_send_buffer(const void *buffer, const size_t size) {
	size_t		i;

	i = 0;
	while (i < size) {
		uart_send(((char *)buffer)[i]);
		++i;
	}
}

void		uart_send(const uint8_t c) {
	outb(uart_addr, c);
}

uint8_t		uart_recv(void) {
	int		interr = inb(uart_addr + 2) & 0x0F;

	if (interr == 0x04) {
		uint8_t c = inb(uart_addr);
		return c;
	}
	return 0;
}
