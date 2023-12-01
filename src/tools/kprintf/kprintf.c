/*
** stdarg_test.c for kprintf in /home/caudou_j/Documents/projets/kprintf
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Mon Nov 11 23:26:28 2013 caudou_j
** Last update Sun Nov 17 12:38:06 2013 caudou_j
*/

#include "kprintf.h"

void format_bytes_size_value(unsigned int value, char *unit_name, int unit_size, char *base) {
    display_uint(value / unit_size);
    if (unit_size != 1) {
        unsigned int decimal = (value % unit_size) * 1000 / unit_size;
        if (decimal > 0) {
            kprintf_putchar('.');
            display_uint((value % unit_size) * 1000 / unit_size);
        }
    }
    my_putstr(unit_name);
}

void format_bytes_size(unsigned int uint) {
    int kb = 1024;
    if (uint < kb)
        format_bytes_size_value(uint, "B", 1, "0123456789");
    else if (uint < kb * kb) {
        format_bytes_size_value(uint, "Kb", kb, "0123456789");
    } else if (uint < kb * kb * kb) {
        format_bytes_size_value(uint, "Mb", kb * kb, "0123456789");
    } else {
        format_bytes_size_value(uint, "Gb", kb * kb * kb, "0123456789");
    }
}

void display_uint(unsigned int uint) {
    if (!uint)
        kprintf_putchar('0');
    else
        my_put_uint_base(uint, "0123456789");
}

void display_pointer(unsigned long ulong) {
    if (ulong) {
        my_putstr("0x");
        my_put_ulong_base(ulong, "0123456789abcdef");
    } else
        my_putstr("(nil)");
}

void display_invalid_flag(char flag) {
    kprintf_putchar('%');
    kprintf_putchar(flag);
}

void	display_var(va_list *ap, const char *str_format, int *cursor)
{
  *cursor = *cursor + 1;
  if (str_format[*cursor] == '%')
    kprintf_putchar('%');
  else if (str_format[*cursor] == 's')
    my_putstr(va_arg(*ap, char *));
  else if (str_format[*cursor] == 'd' || str_format[*cursor] == 'i')
    my_put_nbr_base(va_arg(*ap, int), "0123456789");
  else if (str_format[*cursor] == 'c')
    kprintf_putchar((char)va_arg(*ap, int));
  else if (str_format[*cursor] == 'o')
    my_put_uint_base(va_arg(*ap, unsigned int), "01234567");
  else if (str_format[*cursor] == 'u')
    display_uint(va_arg(*ap, unsigned int));
  else if (str_format[*cursor] == 'x')
    my_put_uint_base(va_arg(*ap, unsigned int), "0123456789abcdef");
  else if (str_format[*cursor] == 'X')
    my_put_uint_base(va_arg(*ap, unsigned int), "0123456789ABCDEF");
  else if (str_format[*cursor] == 'p')
    display_pointer(va_arg(*ap, unsigned long));
  else if (str_format[*cursor] == 'b')
    my_put_nbr_base(va_arg(*ap, unsigned int), "01");
  else if (str_format[*cursor] == 'B')
        format_bytes_size(va_arg(*ap, unsigned int));
  else if (str_format[*cursor] == 'S')
    my_full_putstr(va_arg(*ap, char *));
  else
    display_invalid_flag(str_format[*cursor]);
}

int kprintf(const char *str_format, ...) {
    int cursor;
    va_list ap;

  cursor = 0;
  va_start(ap, str_format);
  while (str_format[cursor])
    {
      if (str_format[cursor] == '%')
        display_var(&ap, str_format, &cursor);
      else
        kprintf_putchar(str_format[cursor]);
      cursor = cursor + 1;
    }
    va_end(ap);
    return (nbr_disp_bytes() - 1);
}
