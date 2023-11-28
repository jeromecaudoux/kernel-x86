#ifndef KB_H_
# define KB_H_

# include "interrupts.h"
# define IO_BUFFER	0x60

void	keyboard_handler(__attribute__((unused))struct regs *regs);
void	kb_init(void);
int	kb_get_key(void);
char	getchar();

#endif /* !KB_H_ */
