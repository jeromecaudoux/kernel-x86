#ifndef UART_H_
# define UART_H_

# include "types.h"

# define ERR_UART_BADADDR "ERR_UART_BADADDR"
# define ERR_UART_ALREADYINIT "ERR_UART_ALREADYINIT"

void		uart_init(const uint16_t addr);
bool		uart_is_init(void);
void		uart_putchar(const uint8_t c);
void		uart_putstr(const char *msg);
void		uart_send(const uint8_t c);
uint8_t		uart_recv(void);
void		uart_send_buffer(const void *buffer, const size_t size);

#endif /* !UART_H_ */
