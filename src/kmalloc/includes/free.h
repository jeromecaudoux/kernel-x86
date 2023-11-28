/*
** free.h for kmalloc in /home/caudou_j/rendu/PSU_year_kmalloc
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Tue Jan 28 15:21:52 2014 caudou_j
** Last update Fri Feb  7 15:43:08 2014 caudou_j
*/

#ifndef		FREE_H_
# define	FREE_H_

/*
** free.c
*/
void	free(void *ptr);

/*
** rm_value.c
*/
t_mem_infos	*merge_freed_ptr(t_page_infos *page_infos,
				 t_mem_infos *mem_infos,
				 t_mem_infos *before);
void		pagesize_rm_update(t_page_infos	*page_infos,
				   t_mem_infos *mem_infos);
void		mem_rm_value(void *ptr);

#endif /* !FREE_H_ */
