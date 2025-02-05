#include "segmentation.h"
#include "string.h"
#include "kprintf.h"

static gdt_entry_t		g_gdt[GDT_SIZE];
static tss_entry_t		g_tss;

int set_gdt_entry(int n, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	if (n >= GDT_SIZE)
		return -1;
	g_gdt[n].limit_low = limit & 0xffff;
	g_gdt[n].base_low = base & 0xffff;
	g_gdt[n].base_mid = (base >> 16) & 0xff;
	g_gdt[n].access.access = access;
	g_gdt[n].limit_high = (limit >> 16) & 0xf;
	g_gdt[n].avl = 0x0;
	g_gdt[n].l = 0x0;
	g_gdt[n].d_b = 0x1;
	g_gdt[n].g = gran; 
	g_gdt[n].base_high = (base >> 24) & 0xff;
	kprintf("GDT[%d]: base: %p, limit: %p, access: %x, granularity: %b\n", n, base, g_gdt[n].limit_low, access, gran);
	return 0;
}

int set_tss_entry(int n, uint16_t ss0, uint32_t esp0)
{
	if (n >= GDT_SIZE)
		return -1;
	uint32_t base = (uint32_t) &g_tss;
	uint32_t limit =  base + sizeof(tss_entry_t);

	set_gdt_entry(n, base, limit, 0xe9, 0x0);
	memset(&g_tss, 0, sizeof(tss_entry_t));
	g_tss.ss0 = ss0;
	g_tss.esp0 = esp0;
	g_tss.cs = 0x08 | 0x3;
	g_tss.ss = g_tss.ds = g_tss.es = g_tss.fs = g_tss.gs = 0x10 | 0x3;
	kprintf("TSS: base: %p, limit: %p, ss0: %p, esp0: %p\n", base, limit, ss0, esp0);
	// kprintf("sizeof(g_tss) %d, limithigh %p, limitlow %pn", sizeof(g_tss), g_gdt[5].limit_high, g_gdt[5].limit_low);
	return 0;
}


/*
 * This function is used to set the tss's esp, so that CPU knows what esp the kernel should be using
 * */
void tss_set_esp(uint32_t esp0) {
    g_tss.esp0 = esp0;
}

static void flush_gdt_and_tss(void) {
	gdt_register_t gdt_register;

	gdt_register.base = (uint32_t) &g_gdt;
	gdt_register.limit = (sizeof(gdt_entry_t) * GDT_SIZE) - 1;

	gdt_flush(&gdt_register);
	tss_flush();	 
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

	/* Userland Code segment */
	access = DPB_CODE_ER | RING_LVL_3 | SEG_PRESENT | DTYPE_CODE_OR_DATA;
	set_gdt_entry(3, SEGMENT_BASE, SEGMENT_LIMIT, access, GRANULARITY_ENABLE);

	/* Userland Data segment */
	access = DPB_DATA_RW | RING_LVL_3 | SEG_PRESENT | DTYPE_CODE_OR_DATA;
	set_gdt_entry(4, SEGMENT_BASE, SEGMENT_LIMIT, access, GRANULARITY_ENABLE);


    uint32_t esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
	set_tss_entry(5, 0x10, esp);

	flush_gdt_and_tss();
}
