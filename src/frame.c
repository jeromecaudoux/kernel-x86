#include "frame.h"
#include "kprintf.h"
#include "string.h"
#include "tools.h"
#include "types.h"
#include "uart.h"

extern int kernel_end;
static t_frame_context g_frames_context;

struct frame *get_frame_at(size_t index)
{
    return (struct frame *) ((unsigned int) FRAMES_START_VADDR +
                             index * sizeof(struct frame));
}

static inline void __native_flush_tlb_single(unsigned int addr)
{
    asm volatile("invlpg (%0)" ::"r"(addr) : "memory");
}

static void init_ram_frames()
{
    struct frame *frame;
    size_t cursor;
    int *fend_vaddr =
        (void *) ((unsigned int) g_frames_context.frames_meta_start_vaddr +
                  g_frames_context.frames_count);
    phys_t fend_paddr = get_physaddr(fend_vaddr);

    memset(g_frames_context.frames_meta_start_vaddr, 0,
           g_frames_context.frames_count * sizeof(struct frame));

    cursor = 0;
    for (cursor = 0; cursor < g_frames_context.frames_count; ++cursor) {
        frame = get_frame_at(cursor);
        phys_t addr = RAM_BASE_ADDR + cursor * FRAME_SIZE;
        frame->ref_count = addr < fend_paddr ? 1 : 0;
        if (frame->ref_count) {
            frame->vaddr = (void *) g_frames_context.frames_meta_start_vaddr +
                           cursor * sizeof(struct frame);
        }
    }
#ifdef DEBUG
    kprintf("Frames metadata in memory : [%p] ... [%p]\n",
            g_frames_context.frames_meta_start_vaddr, fend_paddr);
#endif
}

static void setup_frames_addrs(const memory_map_t *map)
{
    ssize_t ram_free_space = map->length_low - KERNEL_SIZE_IN_RAM;
    size_t nb_frames = ((unsigned long) ram_free_space) / FRAME_SIZE;
    size_t size_of_meta = nb_frames * sizeof(struct frame);
#ifdef DEBUG
    size_t kernel_size =
        (unsigned long) (&kernel_end) - SYSTEM_ADDR_BASE - RAM_BASE_ADDR;

    kprintf("\nKernel size: %B (+ 4MB for the page tables)\n", kernel_size);
    kprintf("RAM has %B of memory available\n", ram_free_space);
    kprintf("RAM can have %d frames [%B / frame]\n", nb_frames, FRAME_SIZE);
    kprintf("Frames map size: %B [%B / frame struct]\n", size_of_meta,
            sizeof(struct frame));

    kprintf("KERNEL_PAGE_DIRECTORY_INDEX: %d\n", KERNEL_PAGE_DIRECTORY_INDEX);
    kprintf("PAGE_TABLES_DIRECTORY_INDEX: %d\n", PAGE_TABLES_DIRECTORY_INDEX);
    kprintf("PAGE_TABLES_START_VADDR: %p\n", PAGE_TABLES_START_VADDR);
    kprintf("FRAMES_START_VADDR: %p\n\n", FRAMES_START_VADDR);
#endif

    g_frames_context.frames_meta_start_vaddr =
        (struct frame *) (FRAMES_START_VADDR);
    g_frames_context.frames_count = nb_frames;

    init_page_directory_for_frames(size_of_meta);
}

void init_frame_alloc(const multiboot_info_t *multiboot_info)
{
    memory_map_t *mmaps = NULL;
    memory_map_t *ram_addr = NULL;
    uint32_t cursor;
    uint32_t size;

    mmaps = (memory_map_t *) (SYSTEM_ADDR_BASE +
                              (unsigned long) multiboot_info->mmap_addr);
    size = multiboot_info->mmap_length / sizeof(memory_map_t);

    kprintf("\nKernel end is at address => %p\n", &kernel_end);
    kprintf("\tMemory found length => %d\n", size);
    kprintf("\tMemory found addr => %p\n\n", mmaps);

    kprintf("\nAvailable memories :\n");
    cursor = 0;
    while (cursor < size) {
        memory_map_t *mmap = &(mmaps[cursor]);
        kprintf("\tMemory found at %p of size %B %x b (%d)\n", mmap->base_addr_low,
                mmap->length_low, mmap->length_low, mmap->type);

        if (mmap->base_addr_low == RAM_BASE_ADDR) {
            ram_addr = mmap;
        }
        ++cursor;
    }

    setup_frames_addrs(ram_addr);
    init_ram_frames();
}

/*
 * Description: Allocate a new frame and return a pointer the structure which
 * describes it.
 */
struct frame *alloc_frame(void)
{
    size_t cursor = 0;
    struct frame *frame;

    while (cursor < g_frames_context.frames_count) {
        frame = get_frame_at(cursor);
        if (frame->ref_count == 0) {
            frame->ref_count = 1;
            return frame;
        }
        ++cursor;
    }
    return NULL;
}

static void frames_set_ref_count(struct frame *frames, int n, int ref_count)
{
    int cursor;

    cursor = 0;
    while (cursor < n) {
        frames->ref_count = ref_count;
        frames = (struct frame *) ((unsigned long) frames + sizeof(struct frame));
        ++cursor;
    }
}

/*
 * Description: Allocate n physically contiguous frames.
 *
 * Arguments:
 * -> n: number of frames to allocate.
 * -> frames: pointer to allocated frames
 */
int alloc_frames(int n, struct frame **frames)
{
    size_t cursor = 0;
    int count = 0;
    struct frame *fsaved = NULL;
    struct frame *fcurrent;

    while (cursor < g_frames_context.frames_count) {
        fcurrent = get_frame_at(cursor);
        //        kprintf("frame %p (ref count %d) at %d at %p\n", fcurrent,
        //        fcurrent->ref_count, cursor,
        //        g_frames_context.frames_meta_start_vaddr);

        if (fcurrent->ref_count == 0) {
            if (fsaved == NULL)
                fsaved = fcurrent;
            ++count;
            if (count == n) {
                *frames = fsaved;
                frames_set_ref_count(fsaved, n, 1);
                return SUCCES;
            }
        }
        else {
            fsaved = NULL;
            count = 0;
        }
        ++cursor;
    }
    return FAILED;
}

/*
 * Description: Decrement reference counter of a frame.
 */
void free_frame(struct frame *frame)
{
    frame->ref_count -= 1;
    if (frame->ref_count < 0) {
        frame->ref_count = 0;
    }
}

/*
 * Description: Get physical address of a frame.
 */
phys_t frame_to_phys(struct frame *f) { return f->addr; }

/*
 * Description: Get frame data from physical address.
 */
struct frame *phys_to_frame(phys_t addr)
{
    phys_t addr_in_frames = addr - 0x800000;
    return get_frame_at(addr_in_frames >> 12);
}
