/*
** kprintf_tools.c for kprintf in /home/caudou_j/Documents/projets/kprintf
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Thu Nov 14 16:33:44 2013 caudou_j
** Last update Sun Nov 17 12:48:47 2013 caudou_j
*/

#include "kprintf.h"
#include "kwrite.h"
#include "stdarg.h"
#include "types.h"

int     my_putstr(char *str)
{
  int   cursor;

  if (str == NULL)
    return (1);
  cursor = 0;
  while (str[cursor])
    {
      kprintf_putchar(str[cursor]);
      cursor = cursor + 1;
    }
  return (0);
}

int	my_full_putstr(char *str)
{
  int	cursor;

  cursor = 0;
  while (str[cursor])
    {
      if (str[cursor] < 32 || str[cursor] >= 127)
	{
	  kprintf_putchar('\\');
	  if (str[cursor] < 32 && str[cursor] >= 8)
	    my_putstr("0");
	  else if (str[cursor] < 8)
	    my_putstr("00");
	  my_put_nbr_base(str[cursor], "01234567");
	}
      else
	kprintf_putchar(str[cursor]);
      cursor = cursor + 1;
    }
  return (0);
}

void	kprintf_putchar(char c)
{
  kwrite(KWRITE_UART, &c, 1);
  nbr_disp_bytes();
}

int		nbr_disp_bytes()
{
  static int	count;

  count = count + 1;
  return (count);
}

int	my_strlen(char *str)
{
  int	cursor;

  cursor = 0;
  while (str[cursor])
    cursor = cursor + 1;
  return (cursor);
}
