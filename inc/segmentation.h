#ifndef SEGMENTATION_H_
# define SEGMENTATION_H_

#include "types.h"

# define GDT_SIZE		4
# define SEGMENT_BASE		0x00000000
# define SEGMENT_LIMIT		0xFFFFFFFF

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
	uint8_t	type : 4;
	uint8_t	s : 1;
	uint8_t	dpl : 2;
	uint8_t	p : 1;
};

union gdt_access {
	uint8_t			 access;
	struct gdt_access_fields fields;
};

struct __attribute__((__packed__)) gdt_entry {
	uint16_t		limit_low : 16;
	uint32_t		base_low : 24;
	union gdt_access	access;
	uint8_t			limit_high : 4;
	uint8_t			avl : 1;
	uint8_t			l : 1;
	uint8_t			d_b : 1;
	uint8_t			g : 1;
	uint8_t			base_high : 8;
};

struct __attribute__((__packed__)) gdt_register {
	uint16_t	limit;
	uint32_t	base;
};

typedef struct gdt_entry gdt_entry_t;
typedef struct gdt_register gdt_register_t;

int set_gdt_entry(int n, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
void init_flat_gdt(void);

#endif /* !SEGMENTATION_H_ */
