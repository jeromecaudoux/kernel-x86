#ifndef PAGES_H_
# define PAGES_H_

#include "frame.h"

struct frame;
typedef unsigned int phys_t;

#define PF_PRES 0x1	/* Is present */
#define PF_RW   0x2	/* Is writeable */
#define PF_USER 0x4 /* Is user-mode */
#define PF_WRTT 0x8 /* Write-through enabled */
#define PF_DISC 0x10	/* Cache disabled */
#define PF_DIRT 0x20	/* Is dirty */
#define PF_ACCS 0x40	/* Has been accessed */
#define PF_4M	0x80	/* 4Mo page */
#define PF_MASK 0x0E7F	/* Page flags that can be used */

#define KERNEL_PAGE_DIRECTORY_INDEX (SYSTEM_ADDR_BASE >> 22)
#define PAGE_TABLES_DIRECTORY_INDEX (KERNEL_PAGE_DIRECTORY_INDEX + 1)
#define FRAMES_DIRECTORY_INDEX (PAGE_TABLES_DIRECTORY_INDEX + 1)

#define PAGE_TABLES_START_VADDR ((t_page_table_entry *)(PAGE_TABLES_DIRECTORY_INDEX << 22))
#define FRAMES_START_VADDR ((struct frame *)(FRAMES_DIRECTORY_INDEX << 22))
#define FRAMES_START_PADDR ((struct frame *) 0x00800000)

#define PDE_4MB_ADDR_HIGH(addr) ((addr) >> 22)
#define PDE_ADDR_HIGH(addr) ((addr) >> 12)
#define PTE_ADDR_HIGH(addr) ((addr) >> 12)
#define PDE_PTADDR_TO_VADDR(addr) (SYSTEM_ADDR_BASE + ((addr) << 12))
#define VADDR_TO_PTADDR(addr) ((unsigned int) addr - (unsigned int) SYSTEM_ADDR_BASE)

// https://wiki.osdev.org/Exceptions#Selector_Error_Code
typedef struct page_fault_handler {
    // 0 The fault was caused by a non-present page.
    // 1 The fault was caused by a page-level protection violation.
#define FAULT_NON_PRESENT_PAGE 0x0
    uint8_t	p : 1;
    // 0 The access causing the fault was a read.
    // 1 The access causing the fault was a write.
    uint8_t	w_r : 1;
    // 0 => The access causing the fault originated when the processor
    // was executing in supervisor mode (CPL < 3).
    // 1 => The access causing the fault originated when the processor
    // was executing in user mode (CPL = 3).
    uint8_t	u_s : 1;
    // 0 => The fault was not caused by reserved bit violation ?
    // 1 => The fault was caused by a reserved bit set to 1 in some paging-structure entry
    uint8_t	rsvd : 1;
    // fault was not caused by an instruction fetch ? Yes => 0x1, No => 0x0
    uint8_t	i_d : 1;
    // reserved
  uint32_t	ignored : 27;
} __attribute__((__packed__)) page_fault_handler_t;

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
void	init_page_directory_for_frames(size_t frames_size);
void	init_page_tables();
phys_t get_physaddr(void *vaddr);
size_t	get_kds_size();

struct frame *get_kpd_frame();

void *sbrk(ssize_t size);
size_t getpagesize();

#endif /* !PAGES_H_ */
