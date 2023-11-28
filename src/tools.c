#include "types.h"
#include "uart.h"

void my_put_nbr(int nb)
{
	int   n;

	n = 0;
	if (nb < 0) {
		uart_putchar('-');
		nb = -nb;
	}

	if (nb > 9)
		my_put_nbr(nb / 10);

	n = nb % 10;
	uart_putchar(n + '0');
}

int strlen(const char *str)
{
	int i = 0;

	if (str == NULL)
		return 0;
	while (str[i] != 0)
		++i;
	return i;
}
