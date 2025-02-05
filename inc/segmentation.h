#ifndef SEGMENTATION_H_
# define SEGMENTATION_H_

#include "types.h"

# define GDT_SIZE		6
# define SEGMENT_BASE		0x00000000  // Kernel and Userland GDT base
# define SEGMENT_LIMIT		0xFFFFFFFF  // Kernel GDT limit
# define USERLAND_LIMIT		0xBFFFFFFF  // Userland GDT limit

# define GRANULARITY_ENABLE	0x1
# define GRANULARITY_DISABLE	0x0

# define SEG_PRESENT		1 << 7
# define SEG_NOT_PRESENT	0 << 7

# define RING_LVL_0		0 << 5
# define RING_LVL_1		1 << 5
# define RING_LVL_2		2 << 5
# define RING_LVL_3		3 << 5

# define DTYPE_SYSTEM		0 << 4
# define DTYPE_CODE_OR_DATA	1 << 4

# define DPB_DATA_RW		2
# define DPB_CODE_ER		10
# define DPB_DATA_RO		0

struct __attribute__((__packed__)) gdt_access_fields {
   bool accessed:1;
   bool read_write:1;
   bool direction_conforming:1;
   bool executable:1;
   bool	s : 1;
   uint8_t	dpl : 2;
   bool	p : 1;
};

union gdt_access {
	uint8_t			 access;
	struct gdt_access_fields fields;
};

struct __attribute__((__packed__)) gdt_entry {
   uint16_t		limit_low : 16;
   uint16_t    base_low;
   uint8_t     base_mid;
	union gdt_access	access;
	uint8_t			limit_high : 4;
	uint8_t			avl : 1;
	uint8_t			l : 1;
	uint8_t			d_b : 1;
	uint8_t			g : 1;
	uint8_t			base_high : 8;
};

struct __attribute__((packed)) tss_entry {
   uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
   uint32_t ss0;        // The stack segment to load when we change to kernel mode.
   uint32_t esp1;       // Unused...
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;         // The value to load into ES when we change to kernel mode.
   uint32_t cs;         // The value to load into CS when we change to kernel mode.
   uint32_t ss;         // The value to load into SS when we change to kernel mode.
   uint32_t ds;         // The value to load into DS when we change to kernel mode.
   uint32_t fs;         // The value to load into FS when we change to kernel mode.
   uint32_t gs;         // The value to load into GS when we change to kernel mode.
   uint32_t ldt;        // Unused...
   uint16_t trap;
   uint16_t iomap_base;
};

struct __attribute__((__packed__)) gdt_register {
	uint16_t	limit;
	uint32_t	base;
};

typedef struct gdt_entry gdt_entry_t;
typedef struct tss_entry tss_entry_t;
typedef struct gdt_register gdt_register_t;

int set_gdt_entry(int n, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
int set_tss_entry(int n, uint16_t ss0, uint32_t esp0);
void init_flat_gdt(void);
void tss_set_esp(uint32_t esp0);
void tss_flush();
void gdt_flush(void* gdt_addr);

#endif /* !SEGMENTATION_H_ */
