/*
** kmalloc.h for kmalloc in /home/caudou_j/rendu/PSU_year_kmalloc
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Tue Jan 28 15:22:22 2014 caudou_j
** Last update Fri Feb  7 16:01:39 2014 caudou_j
*/

#ifndef		MALLOC_H_
# define	MALLOC_H_

# include	"mem_infos.h"
# include	"types.h"

/*
** kmalloc.c
*/
void	*kmalloc(size_t size);

/*
** add_value.c
*/
void		prepare_split_value(t_mem_infos *min_avaible_place,
				    size_t size);
void		*mem_search_place(size_t size);
void		pagesize_add_update(t_page_infos *page_infos,
				    size_t size);
retv		*mem_add_value(size_t size);

#endif /* !MALLOC_H_ */
