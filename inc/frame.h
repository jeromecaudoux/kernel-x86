#ifndef FRAME_H_
# define FRAME_H_

# include	"types.h"
# include	"pages.h"
# include	"main.h"

#define SYSTEM_ADDR_BASE 0xC0000000
#define RAM_BASE_ADDR 0x100000 // 1MB
#define FRAME_SIZE 0x1000 // 4KB
#define FOUR_MB 0x400000 // 4MB

/* 4mb for the page tables + 3mb for the kernel */
#define KERNEL_SIZE_IN_RAM (2 * FOUR_MB - RAM_BASE_ADDR);

typedef unsigned int phys_t;

struct frame
{
  int		ref_count;
  phys_t	addr;
  void		*vaddr;
};

typedef struct	s_frame_context
{
  struct frame *frames_meta_start_vaddr;
  size_t frames_count;
}		t_frame_context;

typedef struct memory_map
{
	unsigned long size;
	unsigned long base_addr_low;
	unsigned long base_addr_high;
	unsigned long length_low;
	unsigned long length_high;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
	unsigned long type;
} memory_map_t;

void init_frame_alloc(const multiboot_info_t *multiboot_info);
struct frame* alloc_frame(void);
int alloc_frames(int n, struct frame **frames);
void free_frame(struct frame *frame);
phys_t frame_to_phys(struct frame *f);
struct frame *phys_to_frame(phys_t physAdress);
struct frame *get_frame_at(size_t index);

#endif /* !FRAME_H_ */
