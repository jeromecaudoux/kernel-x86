/*
** calloc.c for kmalloc in /home/caudou_j/Documents/projets/PSU_2014_kmalloc/calloc
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Tue Feb  4 22:11:21 2014 caudou_j
** Last update Fri Feb  7 15:45:59 2014 caudou_j
*/

#include	<string.h>
#include	"mem_infos.h"
#include	"kmalloc.h"

void		*calloc(size_t nmemb, size_t size)
{
  void		*new;
  size_t	total_size;

  total_size = nmemb * size;
  if ((new = kmalloc(total_size)) == NULL)
    return (NULL);
  memset(new, 0, total_size);
  return (new);
}
