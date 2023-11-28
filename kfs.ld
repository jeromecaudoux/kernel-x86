OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(start)

SECTIONS
{
	. = 0xC0100000 + SIZEOF_HEADERS;
	start = _start - 0xC0000000;
	
	PROVIDE(begin_text = .);
	.text : AT(ADDR(.text) - 0xC0000000)
	{
		*(.multiboot) *(.text)
	}

	.data : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	.rodata : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	.bss : AT(ADDR(.bss) - 0xC0000000)
	{
		*(.bss)
	}
	PROVIDE(kernel_end = .);
}
