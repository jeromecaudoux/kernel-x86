/*
** realloc.c for kmalloc in /home/caudou_j/Documents/projets/PSU_2014_kmalloc/realloc
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Tue Feb  4 12:34:37 2014 caudou_j
** Last update Fri Feb  7 15:45:07 2014 caudou_j
*/

#include	"string.h"
#include	"types.h"
#include	"kprintf.h"
#include	"mem_infos.h"
#include	"kmalloc.h"
#include	"free.h"

void		*realloc(void *ptr, size_t size)
{
  void		*new;
  t_mem_infos	*mem_infos;

  if (size == 0 && ptr != NULL)
    {
      free(ptr);
      return (NULL);
    }
  if (ptr == NULL)
    return (kmalloc(size));
  mem_infos = (t_mem_infos *)((char *)ptr - sizeof(t_mem_infos));
  if (mem_infos->size >= size)
    return (ptr);
  if ((new = kmalloc(size)) == NULL)
    return (NULL);
  memcpy(new, ptr, mem_infos->size);
  return (new);
}
