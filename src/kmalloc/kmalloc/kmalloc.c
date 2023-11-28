/*
** kmalloc.c for kmalloc in /home/caudou_j/rendu/PSU_year_kmalloc/kmalloc
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Tue Jan 28 13:43:25 2014 caudou_j
** Last update Fri Feb  7 15:56:40 2014 caudou_j
*/

#include	"types.h"
#include	"mem_infos.h"
#include	"kmalloc.h"

void	*kmalloc(size_t size)
{
  if (size == 0)
    return (NULL);
  return (mem_add_value(size));
}
