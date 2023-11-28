/*
** my_put_nbr_base.c for kprintf in /home/caudou_j/Documents/projets/kprintf
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Tue Nov 12 12:04:18 2013 caudou_j
** Last update Thu Nov 14 16:41:11 2013 caudou_j
*/

#include "kprintf.h"

int	my_put_nbr_base(int num, char *base)
{
  int	start;
  int	remainder;
  int	len;

  len = my_strlen(base);
  if (num < 0)
    {
      /* if (num == -2147483648) */
      /* 	{ */
      /* 	  my_putstr("-2147483648"); */
      /* 	  return (0); */
      /* 	} */
      kprintf_putchar('-');
      my_put_nbr_base(-num, base);
    }
  else
    {
      remainder = num % len;
      start = (num - remainder) / len;
      if (start != 0)
	my_put_nbr_base(start, base);
      kprintf_putchar(base[remainder]);
    }
  return (0);
}

int	my_put_ulong_base(unsigned long num, char *base)
{
  int	len_base;

  if (num == 0)
    return (0);
  len_base = my_strlen(base);
  my_put_ulong_base(num/len_base, base);
  kprintf_putchar(base[num%len_base]);
  return (0);
}

int	my_put_uint_base(unsigned int num, char *base)
{
  int	len_base;

  if (num == 0)
    return (0);
  len_base = my_strlen(base);
  my_put_uint_base(num/len_base, base);
  kprintf_putchar(base[num%len_base]);
  return (0);
}
