#include	"types.h"
#include	"serial_port.h"
#include	"io.h"

static int is_transmit_empty(void)
{
	return inb(SERIAL_PORT + 5) & 0x20;
}

static int serial_received(void)
{
	return inb(SERIAL_PORT + 5) & 1;
}

void init_serial(void)
{
	/* Disable all interrupts
	   serial_port_outb(SERIAL_PORT + 1, 0x00);
	   Enable DLAB (set baud rate divisor) */
	outb(SERIAL_PORT + 3, 0x80);
	/* Set divisor to 3 (lo byte) 38400 baud */
	outb(SERIAL_PORT + 0, 0x03);
	/*                  (hi byte) */
	outb(SERIAL_PORT + 1, 0x00);
	/* 8 bits, no parity, one stop bit */
	outb(SERIAL_PORT + 3, 0x03);
	/* Enable FIFO, clear them, with 14-byte threshold */
	/* outb(PORT + 2, 0xC7); */
	/* outb(PORT + 4, 0x0B); IRQs enabled, RTS/DSR set */
}

void write_serial(char c)
{
	while (is_transmit_empty() == 0)
		;
	outb(SERIAL_PORT, c);
}

char read_serial(void)
{
	while (serial_received() == 0)
		;
	return inb(SERIAL_PORT);
}

void printk_on_serial_port(const char *string)
{
	int i = 0;

	while (string[i] != NULL) {
		write_serial(string[i]);
		++i;
	}
}
