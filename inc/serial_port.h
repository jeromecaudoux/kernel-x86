#ifndef HEADER_SERIAL_PORT
#define HEADER_SERIAL_PORT

#define	SERIAL_PORT		0x3F8

void	printk_on_serial_port(const char *string);
char	read_serial(void);
void	init_serial(void);
void	write_serial(char c);

#endif
