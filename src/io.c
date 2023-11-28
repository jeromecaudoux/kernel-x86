#include "io.h"

void io_wait(void)
{
  __asm__ volatile ("jmp 1f\n\t"
		 "1:jmp 2f\n\t"
		 "2:" );
}

void outb(unsigned short port, unsigned char val)
{
  __asm__ volatile("outb %0, %1\n\t"
		   : /* No output */
		   : "a" (val), "d" (port));
}

uint8_t inb(uint16_t port)
{
  uint8_t ret;
  __asm__ volatile ("inb %1, %0"
		    : "=a"(ret)
		    : "Nd"(port));
  return ret;
}
