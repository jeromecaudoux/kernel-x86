/*
** mem_infos.c for kmalloc in /home/caudou_j/rendu/PSU_year_kmalloc
**
** Made by caudou_j
** Login   <caudou_j@epitech.net>
**
** Started on  Mon Jan 27 15:58:08 2014 caudou_j
** Last update Fri Feb  7 15:48:50 2014 caudou_j
*/

#include    "pages.h"
#include    "types.h"
#include    "kprintf.h"
#include    "mem_infos.h"

void *get_first_value() {
    static void *first = NULL;
    t_page_infos *page_infos;

    if (first == 0) {
        kprintf("first\n");
        if ((page_infos = sbrk(sizeof(t_page_infos) + getpagesize())) ==
            (void *) -1) { return (NULL); }
        kprintf("first OK??\n");
        first = page_infos;
        page_infos->nb_pages = 1;
        page_infos->max_pos = (t_mem_infos * )((char *) page_infos + sizeof(t_page_infos));
    }
    return (first);
}

void mem_dump() {
    t_mem_infos *mem_infos;
    t_page_infos *page_infos;

    page_infos = get_first_value();
    mem_infos = (t_mem_infos * )((char *) page_infos + sizeof(t_page_infos));

    while (mem_infos && mem_infos < (t_mem_infos * )(page_infos->max_pos)) {
        if (mem_infos->is_free == FALSE) {
            kprintf("%p - %p : %u octets\n", mem_infos->begin, (void *) mem_infos->next,
                    (unsigned int) mem_infos->size);
        }
        mem_infos = mem_infos->next;
    }
}
