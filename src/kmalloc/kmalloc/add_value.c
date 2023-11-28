/*
** add_value.c for kmalloc in /home/caudou_j/Documents/projets/PSU_2014_kmalloc
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Fri Feb  7 15:39:51 2014 caudou_j
** Last update Fri Feb  7 16:08:22 2014 caudou_j
*/

#include	"pages.h"
#include	"kprintf.h"
#include	"types.h"
#include	"mem_infos.h"

void		prepare_split_value(t_mem_infos *min_avaible_place,
				    size_t size)
{
  t_mem_infos	*mem_infos;
  int		free_size;

  min_avaible_place->is_free = FALSE;
  free_size = min_avaible_place->size - size;
  if (free_size > 0 && (size_t)free_size >= sizeof(t_mem_infos) + 1)
    {
      mem_infos = (t_mem_infos *)(((char *)min_avaible_place->begin) + size);
      mem_infos->begin = (char *)mem_infos + sizeof(t_mem_infos);
      mem_infos->is_free = TRUE;
      mem_infos->size = free_size - sizeof(t_mem_infos);
      mem_infos->next = (t_mem_infos *)((char *)mem_infos + sizeof(t_mem_infos) + mem_infos->size);
      min_avaible_place->size = size;
      min_avaible_place->next = mem_infos;
    }
}

void		*mem_search_place(size_t size)
{
  t_mem_infos	*mem_infos;
  t_mem_infos	*min_avaible_place;
  t_page_infos	*page_infos;

  min_avaible_place = NULL;
  page_infos = get_first_value();
  mem_infos = (t_mem_infos *)((char *)page_infos + sizeof(t_page_infos));

  while (mem_infos && mem_infos < (t_mem_infos *)(page_infos->max_pos))
    {
      if (mem_infos->is_free == TRUE)
	if (mem_infos->size >= size && (min_avaible_place == NULL || mem_infos->size < min_avaible_place->size))
	  min_avaible_place = mem_infos;
      mem_infos = mem_infos->next;
    }
  if (min_avaible_place != NULL)
    {
      prepare_split_value(min_avaible_place, size);
      return (min_avaible_place->begin);
    }
  return (NULL);
}

retv		pagesize_add_update(t_page_infos *page_infos,
				    size_t size)
{
  int		nb_pages;

  nb_pages = 0;
  if (((char *)(page_infos->max_pos) + sizeof(t_mem_infos) + size) >
      ((char *)page_infos + sizeof(t_page_infos) + page_infos->nb_pages * getpagesize()))
    {
      nb_pages = ((long)((char *)(page_infos->max_pos) + sizeof(t_mem_infos) + size) -
		  (long)((char *)page_infos + sizeof(t_page_infos) + page_infos->nb_pages * getpagesize())) / getpagesize();
      nb_pages += 1;
    }
  if (nb_pages > 0)
    {
      if (sbrk((nb_pages) * getpagesize()) == (void *)-1)
	{
	  kprintf(ERROR_SBRK_MSG);
	  return (FAILED);
	}
      page_infos->nb_pages += nb_pages;
    }
  return (SUCCES);
}

#include "kprintf.h"

void		*mem_add_value(size_t size)
{
  t_mem_infos	*mem_infos;
  t_page_infos	*page_infos;
  void		*ptr;

  if ((page_infos = get_first_value()) == NULL)
    return (NULL);
  if ((ptr = mem_search_place(size)) != NULL)
    return (ptr);
  if (pagesize_add_update(page_infos, size) == FAILED)
    return (NULL);
  mem_infos = page_infos->max_pos;
  //  kprintf("B 5 (%p)\n", mem_infos);
  mem_infos->size = size;
  // kprintf("B 4\n");
  mem_infos->is_free = FALSE;
  mem_infos->begin = (t_mem_infos *)((char *)mem_infos + sizeof(t_mem_infos));
  mem_infos->next = (t_mem_infos *)((char *)(mem_infos->begin) + size);
  page_infos->max_pos = mem_infos->next;
  return (mem_infos->begin);
}
