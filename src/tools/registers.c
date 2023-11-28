#include "registers.h"

void print_cr0()
{
  int value = 0;

  __asm__ volatile ("mov %%cr0, %%eax;\n\t"
		    "mov %%eax, %0;\n\t"
		   : "=r"(value)
		    : /* No input */
		    : "eax");

  kprintf("cr0 => %p\n", value);
}

void print_cr2()
{
  int value = 0;

  __asm__ volatile ("mov %%cr2, %%eax;\n\t"
		    "mov %%eax, %0;\n\t"
		   : "=r"(value)
		    : /* No input */
		    : "eax");

  kprintf("cr2 => %p\n", value);
}


void print_cr3()
{
  int value = 0;

  __asm__ volatile ("mov %%cr3, %%eax;\n\t"
		    "mov %%eax, %0;\n\t"
		   : "=r"(value)
		    : /* No input */
		    : "eax");

  kprintf("cr3 => %p\n", value);
}

void print_cr4()
{
  int value = 0;

  __asm__ volatile ("mov %%cr4, %%eax;\n\t"
		    "mov %%eax, %0;\n\t"
		   : "=r"(value)
		    : /* No input */
		    : "eax");

  kprintf("cr4 => %p\n", value);
}
