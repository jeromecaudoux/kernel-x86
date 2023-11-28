/*
** rm_value.c for kmalloc in /home/caudou_j/Documents/projets/PSU_2014_kmalloc
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Fri Feb  7 15:41:33 2014 caudou_j
** Last update Fri Feb  7 16:07:56 2014 caudou_j
*/

#include	"pages.h"
#include	"kprintf.h"
#include	"types.h"
#include	"mem_infos.h"

t_mem_infos	*merge_freed_ptr(t_page_infos *page_infos,
				 t_mem_infos *mem_infos,
				 t_mem_infos *before)
{
  t_mem_infos	*after;

  if ((t_mem_infos *)(page_infos->max_pos) > mem_infos)
    {
      after = (t_mem_infos *)((char *)mem_infos + sizeof(t_mem_infos) + mem_infos->size);
      if (after < (t_mem_infos *)(page_infos->max_pos) && after->is_free == TRUE)
	mem_infos->size += sizeof(t_mem_infos) + after->size;
    }
  if (before != mem_infos)
    if (before->is_free == TRUE)
      {
	before->size += sizeof(t_mem_infos) + mem_infos->size;
	return (before);
      }
  return (mem_infos);
}

retv	pagesize_rm_update(t_page_infos	*page_infos,
			   t_mem_infos *mem_infos)
{
  int		nb_pages;

  nb_pages = 0;
  if (mem_infos->next >= (t_mem_infos *)(page_infos->max_pos))
    {
      nb_pages = ((long)((char *)page_infos + sizeof(t_page_infos) + page_infos->nb_pages * getpagesize()) -
		  (long)mem_infos) / getpagesize();
    }
  if (nb_pages > 0)
    {
      if (sbrk(-(nb_pages * getpagesize())) == (void *)-1)
	{
	  kprintf(ERROR_SBRK_MSG);
	  return (FAILED);
	}
      page_infos->nb_pages -= nb_pages;
      page_infos->max_pos = mem_infos;
    }
  return (SUCCES);
}

void		mem_rm_value(void *ptr)
{
  t_mem_infos	*mem_infos;
  t_mem_infos	*last;
  t_page_infos	*page_infos;

  page_infos = get_first_value();
  mem_infos = (t_mem_infos *)((char *)page_infos + sizeof(t_page_infos));
  last = mem_infos;
  while (mem_infos && mem_infos < (t_mem_infos *)(page_infos->max_pos))
    {
      if (mem_infos->begin == ptr)
	{
	  mem_infos = merge_freed_ptr(page_infos, mem_infos, last);
	  mem_infos->is_free = TRUE;
	  mem_infos->begin = (t_mem_infos *)((char *)mem_infos + sizeof(t_mem_infos));
	  mem_infos->next = (t_mem_infos *)((char *)mem_infos->begin + mem_infos->size);
	  pagesize_rm_update(page_infos, mem_infos);
	  return ;
	}
      last = mem_infos;
      mem_infos = mem_infos->next;
    }
}
