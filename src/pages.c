#include "interrupts.h"
#include "pages.h"
#include "kprintf.h"
#include "registers.h"
#include "string.h"

uint32_t page_dir[1024] __attribute__((aligned(4096))); // align 4096
static struct frame g_frame_page_directory;

static size_t g_kds_size; //kernel data segment size for sbrk()

static void		page_fault_handler(__attribute__((unused))struct regs *regs)
{
  kprintf("\n----------------- PAGE FAULT ---------------------\n");
  print_cr2();
  kprintf("Error code => %p\n", regs->err_code);

  __asm__ volatile ("cli\n\t"
		    "hlt\n\t");
}

struct frame *get_kpd_frame()
{
  return &g_frame_page_directory;
}

static void	disable_cache()
{
  __asm__ volatile ("push %eax\n\t"
		    "mov %cr0, %eax;\n\t"
		    "or $(1 << 30), %eax;\n\t"
		    "mov %eax, %cr0;\n\t"
		    "wbinvd\n\t"
		    "pop %eax");
}

static void *get_kds_end_addr(void)
{
  size_t nb_pages;

  nb_pages = g_kds_size / getpagesize() + (g_kds_size % 0x1000 > 0 ? 1 : 0);
  return (void *)(0xc0800000 + nb_pages * getpagesize());
}

void *sbrk(ssize_t size)
{
  struct frame *frames;
  ssize_t nb_pages;
  void *old_kds_end;

  if (size == 0)
    return get_kds_end_addr();
  if (size > 0) {
    nb_pages = size / 0x1000 + (size % 0x1000 > 0 ? 1 : 0);
    if (alloc_frames(nb_pages, & frames) == FAILED)
      return (void *)-1;
    if (map_pages(get_kpd_frame(), get_kds_end_addr(), frames, nb_pages, PF_PRES | PF_RW) == FAILED)
      return (void *)-1;
  } else { // size < 0
    nb_pages = -size / 0x1000 + (-size % 0x1000 > 0 ? 1 : 0);
    unmap_pages(get_kpd_frame(), get_kds_end_addr() - nb_pages * getpagesize(), nb_pages);
  }

  old_kds_end = get_kds_end_addr();
  g_kds_size += size;
  return old_kds_end;
}

size_t getpagesize()
{
  return 0x1000;
}

void   finalize_pagination()
{
  uint32_t *page_directory = (uint32_t *)
    ((unsigned long) &page_dir - (unsigned long) SYSTEM_ADDR_BASE);

  // disable virtual page at 0x0
  page_directory[0] = 0;

  g_frame_page_directory.ref_count = 1;
  g_frame_page_directory.addr = (phys_t)page_directory;
  g_frame_page_directory.vaddr = page_dir;

  // disable mmu cache
  disable_cache();

  // set a page fault handler
  register_irq_handler(14, page_fault_handler);
}

void init_page_tables()
{
  uint16_t cursor;
  t_page_directory_entry *pde;

  kprintf("Page tables in memory : [%p] ... [%p]\n",
	  PAGE_TABLES_START_VADDR, (unsigned long)PAGE_TABLES_START_VADDR + 0x400000);

  // fill the page directory with all page table addrs
  cursor = 0;
  while (cursor < 1024)
    {
      pde = (t_page_directory_entry *)&(page_dir[cursor]);
      if (!(pde->value & PF_PRES))
	{
	  pde->fields.ptaddr = PDE_ADDR_HIGH(0x400000 +
					     cursor * 1024 * sizeof(t_page_table_entry));
	  pde->fields.flags = PF_PRES | PF_RW;
	}
      ++cursor;
    }

  // clear the 4mb page of the page tables : all pte are not present.
  memset(PAGE_TABLES_START_VADDR, 0, 0x400000);
}

void	init_page_directory()
{
  uint32_t *page_directory = (uint32_t *)
    ((unsigned long) &page_dir - (unsigned long) SYSTEM_ADDR_BASE);
  t_page_directory_entry pde;

  memset(page_directory, 0, 1024 * sizeof(uint32_t));

  // Map the kernel at 0x00000000
  // This entry will be deleted once that eip is at 0xc0000000.
  pde.value = 0;
  pde._4mb_fields.addr = PDE_4MB_ADDR_HIGH(0x00000000);
  pde._4mb_fields.flags = PF_PRES | PF_RW | PF_4M;
  page_directory[0] = pde.value;

  // Map the kernel at 0xc0000000
  pde.value = 0;
  pde._4mb_fields.addr = PDE_4MB_ADDR_HIGH(0x00000000);
  pde._4mb_fields.flags = PF_PRES | PF_RW | PF_4M;
  page_directory[KERNEL_PAGE_DIRECTORY_INDEX] = pde.value;

  // All page tables takes 4Mb, so we need a new page of 4mb just after the kernel.
  pde.value = 0;
  pde._4mb_fields.addr = PDE_4MB_ADDR_HIGH(0x400000);
  pde._4mb_fields.flags = PF_PRES | PF_RW | PF_4M;
  page_directory[PAGE_TABLES_DIRECTORY_INDEX] = pde.value;
}

/*
 * Description: find n consecutive free virtual pages and return their address.
 *
 * Arguments:
 * -> pdbr: frame which contains page directory
 * -> n: number of pages needed
 */
void	*alloc_pages(struct frame* pdbr, size_t n)
{
  t_page_directory_entry *pde;
  t_page_table_entry *pte;
  void *vaddr = NULL;
  size_t count = 0;
  uint16_t pde_cursor = 0;
  uint16_t pte_cursor = 0;

  while (pde_cursor < 1024)
    {
      pde = &(((t_page_directory_entry *)pdbr->vaddr)[pde_cursor]);
      if ((pde->value & PF_PRES) && !(pde->value & PF_4M))
	{
	  pte_cursor = 0;
	  while (pte_cursor < 1024)
	    {
	      pte = &(((t_page_table_entry *)
		       (PDE_PTADDR_TO_VADDR(pde->fields.ptaddr)))[pte_cursor]);
	      if (!(pte->value & PF_PRES))
		{
		  ++count;
		  if (count == n)
		    return vaddr;
		}
	      else
		{
		  vaddr = (void *)(pde_cursor * 0x400000 + pte_cursor * 0x1000);
		  count = 0;
		}
	      ++pte_cursor;
	    }
	}
      else
	{
	  vaddr = (void *)(pde_cursor * 0x400000 + pte_cursor * 0x1000);
	  count = 0;
	}
      ++pde_cursor;
    }
  return NULL;
}

static inline void __native_flush_tlb_single(unsigned long addr)
{
  asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

/*
 * Description: Map n physical pages to virtual pages.
 *
 * Arguments:
 * -> pdbr: frame which contains page directory
 * -> vaddr: address of virtual pages
 * -> frames: frames to map
 * -> n: number of frames to map
 * -> flags: access right associated to pages
*/
int	map_pages(struct frame* pdbr, void* vaddr, struct frame *frames, size_t n, int flags)
{
  t_page_table_entry *pte;
  t_page_directory_entry *pde;
  uint16_t pde_cursor;
  uint16_t pte_cursor;
  size_t cursor = 0;

  while (cursor < n)
    {
      pde_cursor = (unsigned long)vaddr / 0x400000;
      pte_cursor = ((unsigned long)vaddr - pde_cursor * 0x400000) / 0x1000;

      pde = &(((t_page_directory_entry *)pdbr->vaddr)[pde_cursor]);
      if ((pde->value & PF_PRES) && !(pde->value & PF_4M))
	{
	  pte = &(((t_page_table_entry *)
		   (PDE_PTADDR_TO_VADDR(pde->fields.ptaddr)))[pte_cursor]);
	  if (pte->value & PF_PRES)
	    {
	      kprintf("e1 %d, %d\n", pde_cursor, pte_cursor);
	      return FAILED; // todo : remove every pages setted previously (or a kernel panic)
	    }

	  // setup the page table entry
	  pte->fields.addr = PTE_ADDR_HIGH(frames->addr);
	  pte->fields.flags = flags;
	  __native_flush_tlb_single((unsigned long)pte);
	  frames->ref_count += 1;

	  // update frames, vaddr and cursor
	  frames = (struct frame *)((unsigned long)frames + sizeof(struct frame));
	  vaddr = (void *)((unsigned long)vaddr + 0x1000);
	  ++cursor;
	}
      else
	{
	  kprintf("e2\n");
	  return FAILED; // todo : remove every pages setted previously (or a kernel panic)
	}
    }
  return SUCCES;
}

/*
 * Description: Map an IO memory range.
 *
 * Arguments:
 * -> pdbr: frame which contains page directory
 * -> ioaddr: base address of IO memory range
 * -> len: size of IO memory range
 *
 * Example: Map VGA text buffer to virtual address space
 * void *video_mem = map_io(pdbr, 0xB8000, 80 * 25 * 2);
 */
void	*map_io(struct frame* pdbr, phys_t ioadddr, size_t len)
{
  void *vaddr;
  struct frame frame;
  size_t nb_pages;
  size_t cursor;

  nb_pages = (len / 0x1000) + (len % 0x1000 > 0 ? 1 : 0);

  frame.addr = ioadddr & 0xFFFFF000;
  vaddr = alloc_pages(pdbr, nb_pages);

  cursor = 0;
  while (cursor < nb_pages)
    {
      map_pages(pdbr, (void *)((unsigned long)vaddr + cursor * 0x1000),
		&frame, 1, PF_PRES | PF_RW);
      frame.addr += 0x1000;
      ++cursor;
    }
  return vaddr + ioadddr - (ioadddr & 0xFFFFF000);
}

/*
 * Description: Unmap pages.
 *
 * Arguments:
 * -> pbdr: frame which contains page directory
 * -> vaddr: address of pages to unmap
 * -> n: number of pages to unmap
 */
// tested
void	unmap_pages(struct frame *pdbr, void *vaddr, int n)
{
  t_page_table_entry *pte;
  t_page_directory_entry *pde;
  uint16_t pde_cursor;
  uint16_t pte_cursor;
  size_t cursor = 0;

  while (cursor < n)
    {
      pde_cursor = (unsigned long)vaddr / 0x400000;
      pte_cursor = ((unsigned long)vaddr - pde_cursor * 0x400000) / 0x1000;

      pde = &(((t_page_directory_entry *)pdbr->vaddr)[pde_cursor]);
      pte = &(((t_page_table_entry *)
	       (PDE_PTADDR_TO_VADDR(pde->fields.ptaddr)))[pte_cursor]);

      // clear the page table entry
      pte->value = 0x0;

      // update vaddr and cursors
      vaddr = (void *)((unsigned long)vaddr + 0x1000);
      ++cursor;
    }
}
