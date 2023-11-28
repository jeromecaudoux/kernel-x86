/*
** free.c for kmalloc in /home/caudou_j/rendu/PSU_year_kmalloc/free
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Tue Jan 28 13:38:29 2014 caudou_j
** Last update Fri Feb  7 15:49:44 2014 caudou_j
*/

#include	"types.h"
#include	"mem_infos.h"
#include	"free.h"

void	free(void *ptr)
{
  mem_rm_value(ptr);
}
