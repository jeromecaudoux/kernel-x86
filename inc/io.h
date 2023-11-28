#ifndef IO_H_
# define IO_H_

#include "types.h"

void io_wait(void);
void outb(unsigned short port, unsigned char val);
uint8_t inb(uint16_t port);

#endif /* !IO_H_ */
