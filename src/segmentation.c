#include "segmentation.h"
#include "string.h"

static gdt_entry_t		g_gdt[GDT_SIZE];

int set_gdt_entry(int n, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
	if (n >= GDT_SIZE)
		return -1;
	g_gdt[n].limit_low = limit & 0x0000FFFF;
	g_gdt[n].base_low = base & 0x00FFFFFF;
	g_gdt[n].access.access = access;
	g_gdt[n].limit_high = (limit & 0x000F0000) >> 16;
	g_gdt[n].avl = 0x0;
	g_gdt[n].l = 0x0;
	g_gdt[n].d_b = 0x1;
	g_gdt[n].g = granularity;
	g_gdt[n].base_high = (base & 0xFF000000) >> 24;
	return 0;
}

static void reload_segment_selectors(void) {
        asm volatile ("movw $0x10, %ax \n"
                      "movw %ax, %ds \n"
                      "movw %ax, %es \n"
                      "movw %ax, %fs \n"
                      "movw %ax, %gs \n"
                      "movw %ax, %ss \n");

        asm volatile ("pushl $0x08 \n"
                      "pushl $1f \n"
                      "lret \n"
                      "1:");
}

static void load_gdt_register(void) {
	gdt_register_t gdt_register;

	gdt_register.base = (uint32_t)g_gdt;
	gdt_register.limit = sizeof(g_gdt) - 1;

	__asm__ volatile("lgdt %0\n"
			 : /* no output */
			 : "m" (gdt_register)
			 : "memory");
}

void init_flat_gdt(void)
{
	uint8_t access;

	/* Null segment */
	access = DPB_DATA_RO | RING_LVL_0 | SEG_NOT_PRESENT | DTYPE_SYSTEM;
	set_gdt_entry(0, SEGMENT_BASE, SEGMENT_BASE, access, GRANULARITY_DISABLE);

	/* Code segment */
	access = DPB_CODE_ER | RING_LVL_0 | SEG_PRESENT | DTYPE_CODE_OR_DATA;
	set_gdt_entry(1, SEGMENT_BASE, SEGMENT_LIMIT, access, GRANULARITY_ENABLE);

	/* Data segment */
	access = DPB_DATA_RW | RING_LVL_0 | SEG_PRESENT | DTYPE_CODE_OR_DATA;
	set_gdt_entry(2, SEGMENT_BASE, SEGMENT_LIMIT, access, GRANULARITY_ENABLE);

	load_gdt_register();
	reload_segment_selectors();
}
