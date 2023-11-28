#include	"types.h"
#include	"frame.h"
#include	"uart.h"
#include	"tools.h"
#include	"kprintf.h"
#include	"string.h"

extern int kernel_end;
static t_frame_context g_frames_context;

static struct frame *get_frame_at(size_t index)
{
  return (struct frame *)((unsigned long)g_frames_context.frames_meta_start_vaddr
			  + index * sizeof(struct frame));
}

static void init_ram_frames() {
  size_t cursor = 0;
  unsigned long addr;
  struct frame *frame;

  addr = 0x800000;
  while (cursor < g_frames_context.frames_count) {
    frame = get_frame_at(cursor);

    frame->ref_count = 0;
    frame->addr = addr;

    addr += FRAME_SIZE;
    ++cursor;
  }
  kprintf("Frames metadata in memory : [%p] ... [%p]\n",
	  g_frames_context.frames_meta_start_vaddr,
	  ((unsigned long)g_frames_context.frames_meta_start_vaddr
	   + g_frames_context.frames_count * sizeof(struct frame)));
}

static void setup_frames_addrs(const memory_map_t *map) {
  size_t kernel_size = (unsigned long)(&kernel_end) - SYSTEM_ADDR_BASE - RAM_BASE_ADDR;
  ssize_t ram_free_space = map->length_low - KERNEL_SIZE_IN_RAM;
  size_t nb_frames = ram_free_space / FRAME_SIZE;
  size_t size_of_meta = nb_frames * sizeof(struct frame);

  kprintf("\nKernel size : %d KB [0x%x B] (+ 4MB for the page tables)\n",
	  kernel_size / 1024, kernel_size);
  kprintf("RAM has %d MB of memory available [0x%x B]\n",
	  ram_free_space >> 20, ram_free_space);
  kprintf("RAM can have %d frames [0x%x B / frame]\n",
	  nb_frames, FRAME_SIZE);
  kprintf("Frames metadata size : %d KB [%d B / frame struct] [0x%x B]\n",
	  size_of_meta / 1024, sizeof(struct frame), size_of_meta);

  g_frames_context.frames_meta_start_vaddr =
        (struct frame *)(SYSTEM_ADDR_BASE + map->base_addr_low + kernel_size);
  g_frames_context.frames_count = nb_frames;
}

void init_frame_alloc(const multiboot_info_t *multiboot_info)
{
  memory_map_t *mmaps = NULL;
  memory_map_t *ram_addr = NULL;
  uint32_t cursor;
  uint32_t size;

  uart_putstr("Frame allocator init...\n");
  mmaps = (memory_map_t *)(SYSTEM_ADDR_BASE + (unsigned long)multiboot_info->mmap_addr);
  size = multiboot_info->mmap_length / sizeof(memory_map_t);

  kprintf("\nInfos :\n");
  kprintf("\n\tKernel end is at address => %p\n", &kernel_end);
  kprintf("\tMemory found length => %d\n", size);
  kprintf("\tMemory found addr => %p\n\n", mmaps);

  kprintf("\nAvailable memories :\n\n");
  cursor = 0;
  while (cursor < size) {
    kprintf("\tMemory found at %p of size %x\n",
	    mmaps[cursor].base_addr_low,
	    mmaps[cursor].length_low);

    if (mmaps[cursor].base_addr_low == RAM_BASE_ADDR) {
      ram_addr = &(mmaps[cursor]);
    }
    ++cursor;
  }

  setup_frames_addrs(ram_addr);
  init_ram_frames();
}

/*
 * Description: Allocate a new frame and return a pointer the structure which describes it.
 */
struct frame* alloc_frame(void)
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
  while (cursor < n)
    {
      frames->ref_count = ref_count;
      frames = (struct frame *)((unsigned long)frames + sizeof(struct frame));
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

    if (fcurrent->ref_count == 0)
      {
	if (fsaved == NULL)
	  fsaved = fcurrent;
	++count;
	if (count == n)
	  {
	    *frames = fsaved;
	    frames_set_ref_count(fsaved, n, 1);
	    return SUCCES;
	  }
      }
    else
      {
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
phys_t frame_to_phys(struct frame *f)
{
  return f->addr;
}

/*
 * Description: Get frame data from physical address.
 */
struct frame *phys_to_frame(phys_t addr)
{
  phys_t addr_in_frames = addr - 0x800000;
  return get_frame_at(addr_in_frames >> 12);
}
