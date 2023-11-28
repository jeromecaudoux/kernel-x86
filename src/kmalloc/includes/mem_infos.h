/*
** mem_infos.h for kmalloc in /home/caudou_j/rendu/PSU_year_kmalloc/mem_infos
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Mon Jan 27 16:59:15 2014 caudou_j
** Last update Fri Feb  7 16:06:05 2014 caudou_j
*/

#ifndef		MEM_INFOS_H_
# define	MEM_INFOS_H_

# define	ERROR_SBRK_MSG	"kmalloc/free Error : sbrk() failed\n"

typedef struct		s_mem_infos
{
  size_t		size;
  bool			is_free;
  void			*begin;
  struct s_mem_infos	*next;
}			t_mem_infos;

typedef struct		s_page_infos
{
  size_t		nb_pages;
  void			*max_pos;
}			t_page_infos;

/*
** mem_infos.c
*/
void		*get_first_value();
void		mem_dump();

#endif /* !MEM_INFOS_H_ */
