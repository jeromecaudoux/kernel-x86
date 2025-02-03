#include "pages.h"
#include "frame.h"
#include "interrupts.h"
#include "kprintf.h"
#include "registers.h"
#include "string.h"

uint32_t page_dir[1024] __attribute__((aligned(4096))); // align 4096
static struct frame g_frame_page_directory;

static size_t g_kds_size; // kernel data segment size for sbrk()

size_t get_kds_size()
{
    return g_kds_size;
}

static inline void __native_flush_tlb_single(unsigned long addr)
{
    asm volatile("invlpg (%0)" ::"r"(addr) : "memory");
}

static void page_fault_handler(__attribute__((unused)) struct regs *regs)
{
    struct frame *frame = NULL;
    int cr2 = print_cr2();
    page_fault_handler_t *handler = (page_fault_handler_t *) &(regs->err_code);

#ifdef DEBUG
    kprintf("\n----------------- PAGE FAULT ---------------------\n");
    kprintf("cr2 => %p\n", cr2);
    kprintf("Error code => %b\n", regs->err_code);
    kprintf("i_d => %d\n", handler->i_d);
    kprintf("rsvd => %d\n", handler->rsvd);
    kprintf("u_s => %d\n", handler->u_s);
    kprintf("w_r => %d\n", handler->w_r);
    kprintf("p => %d\n", handler->p);
    kprintf("--------------------------------------------------\n");
#endif

    while (1)
        ;

    if (handler->p == FAULT_NON_PRESENT_PAGE) {
        alloc_frames(1, &frame);
        map_pages(get_kpd_frame(), (void *) cr2, frame, 1, PF_PRES | PF_RW | PF_4M);
        return;
    }
    __asm__ volatile("cli\n\t"
                     "hlt\n\t");
}

struct frame *get_kpd_frame()
{
    return &g_frame_page_directory;
}

static void disable_cache()
{
    __asm__ volatile("push %eax\n\t"
                     "mov %cr0, %eax;\n\t"
                     "or $(1 << 30), %eax;\n\t"
                     "mov %eax, %cr0;\n\t"
                     "wbinvd\n\t"
                     "pop %eax");
}

static void *get_kds_end_addr(void)
{
    size_t nb_pages;

    nb_pages = g_kds_size / getpagesize() + (g_kds_size % FRAME_SIZE > 0 ? 1 : 0);
    return (void *) ((void *) 0xc08000000 + nb_pages * getpagesize());
}

void *sbrk(ssize_t size)
{
    struct frame *frames;
    ssize_t nb_pages;
    void *old_kds_end;

    kprintf("sbrk: %d\n", size);
    if (size == 0)
        return get_kds_end_addr();
    if (size > 0) {
        nb_pages = size / FRAME_SIZE + (size % FRAME_SIZE > 0 ? 1 : 0);

        uint32_t max_nb_pages = get_frames_context()->map->base_addr_low + get_frames_context()->map->length_low / FRAME_SIZE;
        uint32_t next_nb_pages = get_kds_size() / FRAME_SIZE + nb_pages;
        // kprintf("get_kds_end_addr(): %p\n", get_kds_end_addr());
        // kprintf("max: %p\n", (void *) max);
        if (next_nb_pages > max_nb_pages) {
            kprintf("sbrk failed: not enough memory\n");
            return (void *) -1;
        }
        if (alloc_frames(nb_pages, &frames) == FAILED)
            return (void *) -1;
        if (map_pages(get_kpd_frame(), get_kds_end_addr(), frames, nb_pages, PF_PRES | PF_RW) ==
            FAILED)
            return (void *) -1;
    }
    else { // size < 0
        nb_pages = -size / FRAME_SIZE + (-size % FRAME_SIZE > 0 ? 1 : 0);
        unmap_pages(get_kpd_frame(), get_kds_end_addr() - nb_pages * getpagesize(), nb_pages);
    }

    old_kds_end = get_kds_end_addr();
    g_kds_size += size;
    return old_kds_end;
}

size_t getpagesize()
{
    return FRAME_SIZE;
}

void finalize_pagination()
{
    uint32_t *page_directory = (uint32_t *) ((unsigned long) &page_dir - (unsigned long) SYSTEM_ADDR_BASE);

    // disable virtual page at 0x0
    page_directory[0] = 0;

    g_frame_page_directory.ref_count = 1;
    g_frame_page_directory.addr = (phys_t) page_directory;
    g_frame_page_directory.vaddr = page_dir;

    // disable mmu cache
    disable_cache();

    // set a page fault handler
    register_irq_handler(14, page_fault_handler);
}

/*
 * http://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.html
 */
void init_page_tables()
{
    uint16_t pde_cursor;
    t_page_directory_entry *pde;

#ifdef DEBUG
    kprintf("Page tables in memory : [%p] ... [%p]\n", PAGE_TABLES_START_VADDR,
            (unsigned long) PAGE_TABLES_START_VADDR + 0x400000);
#endif

    // fill the page directory with all page table addrs
    pde_cursor = 0;
    while (pde_cursor < 1024) {
        pde = ((t_page_directory_entry *) g_frame_page_directory.addr) + pde_cursor;
        if (!(pde->value & PF_PRES)) {
            pde->fields.ptaddr = PDE_ADDR_HIGH(0x00400000 + pde_cursor * 1024 * sizeof(t_page_table_entry));
            // pde->fields.flags = PF_RW | PF_4M;
        }
        ++pde_cursor;
    }
    // clear the 4mb page of the page tables : all pte are not present.
    memset(PAGE_TABLES_START_VADDR, 0, 0x400000);
}

void init_page_directory()
{
    uint32_t *page_directory = (uint32_t *) ((unsigned long) &page_dir - (unsigned long) SYSTEM_ADDR_BASE);
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

    // The PDEs table takes 4Mb, so we need a new page of 4mb just after the kernel.
    pde.value = 0;
    pde._4mb_fields.addr = PDE_4MB_ADDR_HIGH(0x00400000);
    pde._4mb_fields.flags = PF_PRES | PF_RW | PF_4M;
    page_directory[PAGE_TABLES_DIRECTORY_INDEX] = pde.value;
}

phys_t get_physaddr(void *vaddr)
{
    phys_t pdindex = (phys_t) vaddr >> 22;
    t_page_directory_entry *pd = (t_page_directory_entry *) &(page_dir[pdindex]);

#ifdef DEBUG
    kprintf("\n[V2Paddr] vaddr: %p\n", vaddr);
    kprintf("[V2Paddr] PDE index: %d\n", pdindex);
    kprintf("[V2Paddr] PDE present: %d\n", pd->value & PF_PRES);
    kprintf("[V2Paddr] PDE is 4mb: %d\n", pd->value & PF_4M);
#endif

    if (!(pd->value & PF_PRES)) {
        return NULL;
    }
    if (pd->value & PF_4M) {
        if (pd->_4mb_fields.ignored || pd->_4mb_fields.ignored_) {
            return NULL;
        }
        phys_t offset_within_page = (phys_t) vaddr & 0xFFF;
#ifdef DEBUG
        kprintf("[V2Paddr] Offset_within_page: %p\n", (void *) offset_within_page);
#endif
        return (pd->_4mb_fields.addr << 22) + offset_within_page;
    }

    t_page_table_entry *pt = (t_page_table_entry *) PDE_PTADDR_TO_VADDR(pd->fields.ptaddr);
    phys_t ptindex = (phys_t) vaddr >> 12 & 0x03FF;
    phys_t offset_within_page = (phys_t) vaddr & 0xFFF;

    pt = &pt[ptindex];
#ifdef DEBUG
    kprintf("[V2Paddr] PTE index: %d\n", ptindex);
    kprintf("[V2Paddr] PTE present: %d\n", pt->value & PF_PRES);
    kprintf("[V2Paddr] Offset_within_page: %p\n", (void *) offset_within_page);
#endif

    if (!(pt->value & PF_PRES) || pt->fields.ignored) {
        return NULL;
    }

#ifdef DEBUG
    kprintf("[V2Paddr] PDE flags: %b\n", pd->fields.flags);
    kprintf("[V2Paddr] PTE flags: %b\n", pt->fields.flags);
    kprintf("[V2Paddr] vaddr %p is mapped to paddr %p\n", vaddr, (pt->fields.addr << 12) + offset_within_page);
#endif

    return (pt->fields.addr << 12) + offset_within_page;
}

extern void flush_tlb(void);
/*
 * Setup the page directory entries for the frames.
 * This function exist because we need to enable the pde and pte before writing the frames into the
 * memory.
 * Probably its possible to handle this through the page fault handler.
 */
void init_page_directory_for_frames(size_t frames_size)
{
    int pde_count = (frames_size / FOUR_MB) + (frames_size % FOUR_MB > 0 ? 1 : 0);
    t_page_directory_entry *pde;
    t_page_table_entry *pte;
    uint16_t cursor = 0;
    uint16_t pde_cursor = 0;
    uint16_t pte_cursor = 0;

    while (cursor < pde_count) {
        pde_cursor = FRAMES_DIRECTORY_INDEX + cursor;
        pde = (t_page_directory_entry *) &(page_dir[pde_cursor]);
        // todo PF_RW is not needed. To check when the user land is implemented.
        pde->fields.flags = PF_PRES | PF_RW;

        uint16_t max_last_pde = (frames_size % FOUR_MB) / FRAME_SIZE + (frames_size % FRAME_SIZE > 0 ? 1 : 0);
        pte_cursor = 0;
        while (pte_cursor < 1024 && (cursor < pde_count - 1 || pte_cursor < max_last_pde)) {
            pte = (t_page_table_entry *) PDE_PTADDR_TO_VADDR(pde->fields.ptaddr);
            pte = &pte[pte_cursor];
            unsigned int faddr = ((unsigned int) FRAMES_START_PADDR) + cursor * FOUR_MB + pte_cursor * FRAME_SIZE;
            pte->value = 0;
            pte->fields.addr = PTE_ADDR_HIGH(faddr);
            pte->fields.flags = PF_PRES | PF_RW;
            __native_flush_tlb_single((unsigned long) &pte[pte_cursor]);

            ++pte_cursor;
        }
        ++cursor;
    }

#ifdef DEBUG
    kprintf("Control: vaddr 0xc08000000 is mapped to paddr %p\n", get_physaddr((void *) 0xc0800000));
#endif

    // update the global count of allocated pages
    // g_kds_size = pde_count * 1024;
}

/*
 * Description: find n consecutive free virtual pages and return their address.
 *
 * Arguments:
 * -> pdbr: frame which contains page directory
 * -> n: number of pages needed
 */
void *alloc_pages(struct frame *pdbr, size_t n)
{
    t_page_directory_entry *pde;
    t_page_table_entry *pte;
    void *vaddr = NULL;
    size_t count = 0;
    uint32_t fvaddr = (uint32_t) get_frames_context()->frames_meta_end_vaddr;
    uint16_t pde_cursor = fvaddr / FOUR_MB;
    uint16_t pte_cursor = ((fvaddr - pde_cursor * FOUR_MB) / FRAME_SIZE) + 1;

    while (pde_cursor < 1024) {
        pde = &(((t_page_directory_entry *) pdbr->vaddr)[pde_cursor]);
        if ((pde->value & PF_PRES) && !(pde->value & PF_4M)) {
            pte_cursor = 0;
            while (pte_cursor < 1024) {
                pte = &((t_page_table_entry *) PDE_PTADDR_TO_VADDR(pde->fields.ptaddr))[pte_cursor];
                if (!(pte->value & PF_PRES)) {
                    ++count;
                    if (count == n)
                        return vaddr;
                }
                else {
                    vaddr = (void *) (pde_cursor * 0x400000 + pte_cursor * 0x1000);
                    count = 0;
                }
                ++pte_cursor;
            }
        }
        else {
            vaddr = (void *) (pde_cursor * 0x400000 + pte_cursor * 0x1000);
            count = 0;
        }
        ++pde_cursor;
    }
    return NULL;
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
int map_pages(struct frame *pdbr, void *vaddr, struct frame *frames, size_t n, int flags)
{
    t_page_table_entry *pte;
    t_page_directory_entry *pde;
    uint16_t pde_cursor;
    uint16_t pte_cursor;
    size_t cursor = 0;

kprintf("map_pages: vaddr %p, frames %p, n %d, flags %b\n", vaddr, frames, n, flags);
    while (cursor < n) {
        pde_cursor = (unsigned long) vaddr / FOUR_MB;
        pte_cursor = ((unsigned long) vaddr - pde_cursor * FOUR_MB) / FRAME_SIZE;

        pde = &(((t_page_directory_entry *) pdbr->vaddr)[pde_cursor]);
        if (!(pde->value & PF_PRES)) {
            pde->fields.flags = PF_PRES | PF_RW;
        }
        if ((pde->value & PF_PRES) && !(pde->value & PF_4M)) {
            pte = &(((t_page_table_entry *) (PDE_PTADDR_TO_VADDR(pde->fields.ptaddr)))[pte_cursor]);
                // kprintf("GOING FOR pde %d, pte %d pte->value: %b\n", pde_cursor, pte_cursor, pte->value);
            if (pte->value & PF_PRES) {
                kprintf("map_pages failed: pde %d, pte %d pte->value: %b\n", pde_cursor, pte_cursor, pte->value);
                return FAILED; // todo : remove every pages setted previously (or a kernel panic)
            }

            // setup the page table entry
            pte->fields.addr = PTE_ADDR_HIGH(frames->addr);
            pte->fields.flags = flags;
            __native_flush_tlb_single((unsigned long) pte);

            frames->ref_count += 1;
            frames->vaddr = vaddr;

            // update frames, vaddr and cursor
            frames = (struct frame *) ((unsigned long) frames + sizeof(struct frame));
            vaddr = (void *) ((unsigned long) vaddr + FRAME_SIZE);
            ++cursor;
        }
        else {
            kprintf("map_pages failed: pde %d pde->value %b\n", pde_cursor, pde->value);
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
void *map_io(struct frame *pdbr, phys_t ioadddr, size_t len)
{
    void *vaddr;
    struct frame frame;
    size_t nb_pages;
    size_t cursor;

    nb_pages = (len / FRAME_SIZE) + (len % FRAME_SIZE > 0 ? 1 : 0);

    frame.addr = ioadddr & 0xFFFFF000;
    vaddr = alloc_pages(pdbr, nb_pages);

    cursor = 0;
    while (cursor < nb_pages) {
        map_pages(pdbr, (void *) ((unsigned long) vaddr + cursor * FRAME_SIZE), &frame, 1,
                  PF_PRES | PF_RW);
        frame.addr += FRAME_SIZE;
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
void unmap_pages(struct frame *pdbr, void *vaddr, int n)
{
    t_page_table_entry *pte;
    t_page_directory_entry *pde;
    uint16_t pde_cursor;
    uint16_t pte_cursor;
    size_t cursor = 0;

    while (cursor < n) {
        pde_cursor = (unsigned long) vaddr / 0x400000;
        pte_cursor = ((unsigned long) vaddr - pde_cursor * 0x400000) / 0x1000;

        pde = &(((t_page_directory_entry *) pdbr->vaddr)[pde_cursor]);
        pte = &(((t_page_table_entry *) (PDE_PTADDR_TO_VADDR(pde->_4mb_fields.addr)))[pte_cursor]);

        // clear the page table entry
        pte->value = 0x0;

        // update vaddr and cursors
        vaddr = (void *) ((unsigned long) vaddr + 0x1000);
        ++cursor;
    }
}
