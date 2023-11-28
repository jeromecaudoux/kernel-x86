#ifndef PAGES_H_
# define PAGES_H_

#include "frame.h"

struct frame;
typedef unsigned int phys_t;

#define PF_PRES 0x1	/* Is present */
#define PF_RW   0x2	/* Is writeable */
#define PF_USER 0x4	/* Is user-mode */
#define PF_WRTT 0x8	/* Write-through enabled */
#define PF_DISC 0x10	/* Cache disabled */
#define PF_DIRT 0x20	/* Is dirty */
#define PF_ACCS 0x40	/* Has been accessed */
#define PF_4M	0x80	/* 4Mo page */
#define PF_MASK 0x0E7F	/* Page flags that can be used */

#define KERNEL_PAGE_DIRECTORY_INDEX (SYSTEM_ADDR_BASE >> 22)
#define PAGE_TABLES_DIRECTORY_INDEX (KERNEL_PAGE_DIRECTORY_INDEX + 1)
#define FRAMES_DIRECTORY_INDEX (PAGE_TABLES_DIRECTORY_INDEX + 1)

#define PAGE_TABLES_START_VADDR ((t_page_table_entry *)(PAGE_TABLES_DIRECTORY_INDEX << 22))

#define PDE_4MB_ADDR_HIGH(addr) ((addr) >> 22)
#define PDE_ADDR_HIGH(addr) ((addr) >> 12)
#define PTE_ADDR_HIGH(addr) ((addr) >> 12)
#define PDE_PTADDR_TO_VADDR(addr) (SYSTEM_ADDR_BASE + ((addr) << 12))

struct __attribute__((__packed__)) pde_fields_4mb_page {
  uint16_t	flags : 9;
  uint8_t	ignored : 3;
  uint8_t	pat : 1;
  uint16_t	ignored_ : 9;
  uint32_t	addr : 10;
};

struct __attribute__((__packed__)) pde_fields_page_table {
  uint8_t	flags : 8;
  uint8_t	ignored : 4;
  uint32_t	ptaddr : 20;
};

struct __attribute__((__packed__)) pte_fields {
  uint16_t	flags : 9;
  uint8_t	ignored : 3;
  uint32_t	addr : 20;
};

typedef union	e_page_directory_entry
{
  struct pde_fields_4mb_page	_4mb_fields;
  struct pde_fields_page_table	fields;
  uint32_t			value;
}		t_page_directory_entry;

typedef union	e_page_table_entry
{
  struct pte_fields	fields;
  uint32_t		value;
}		t_page_table_entry;

void	*alloc_pages(struct frame* pdbr, size_t n);
int	map_pages(struct frame* pdbr, void* vaddr, struct frame *frames, size_t n, int flags);
void	*map_io(struct frame* pdbr, phys_t ioadddr, size_t len);
void	unmap_pages(struct frame *pdbr, void *vaddr, int n);

void	finalize_pagination();
void	init_page_directory();
void	init_page_tables();

struct frame *get_kpd_frame();

void *sbrk(ssize_t size);
size_t getpagesize();

#endif /* !PAGES_H_ */
