/*
** kprintf.h for kprintf in /home/caudou_j/Documents/projets/kprintf
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Thu Nov 14 16:36:17 2013 caudou_j
** Last update Sun Nov 17 12:46:52 2013 caudou_j
*/

#ifndef MY_PRINTF_H_
# define MY_PRINTF_H_

# include "stdarg.h"

void	display_uint(unsigned int uint);
void	display_pointer(unsigned long ulong);
void	display_var(va_list *ap, const char *str_format, int *cursor);
int	kprintf(const char *str_format, ...);
int	my_put_nbr_base(int num, char *base);
int	my_put_ulong_base(unsigned long num, char *base);
int	my_put_uint_base(unsigned int num, char *base);
int     my_putstr(char *str);
int	my_full_putstr(char *str);
void	kprintf_putchar(char c);
int	nbr_disp_bytes();
int	my_strlen(char *str);


#endif /* !MY_PRINTF_H_ */
