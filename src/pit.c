#include "pit.h"
#include "irq.h"
#include "io.h"
#include "uart.h"
#include "tools.h"

static uint32_t ticks = 0;
void pit_handler(__attribute__((unused))struct regs *regs)
{
	++ticks;
	/* my_put_nbr(pit_get_ticks()); */
	/* uart_putchar('\n'); */
}

int pit_init(int freq)
{
	uint32_t divisor = FREQUENCY / freq;

	irq_unmask(0);
	outb(COUNTER_REG, SC_0 | MODE_2);
	outb(COUNTER_0, divisor & 0x00ff);
	outb(COUNTER_0, (divisor >> 8) & 0x00ff);

	register_irq_handler(32, &pit_handler);
	return 0;
}


uint32_t pit_get_ticks(void)
{
	return ticks;
}
