#define MAGIC 0x1badb002
#define FLAGS 0x7
#define KERNEL_VIRTUAL_ADDRESS 0xc0000000
#define STACK_SIZE 0x10000

.section .data
stack:
	   .space STACK_SIZE, 0x00

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long -(MAGIC + FLAGS)

.section .text
.global _start

_start:
	/* setup the stack */
	mov $(stack + STACK_SIZE), %esp
	sub $0xc0000000, %esp

	/* jump */
	push %ebx /* multiboot info */
	push %eax /* magic */
	call main

	/*jmp main*/
	hlt

.global enable_paging
enable_paging:
	/* enable 4Mb pages */
	push %eax
	mov %cr4, %eax
	or $0x00000010, %eax
	mov %eax, %cr4
	pop %eax

	/* update cr3 */
	push %eax
	mov $page_dir, %eax
	sub $0xc0000000, %eax
	mov %eax, %cr3
	pop %eax

	/* enable paging */
	push %eax
	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0
	pop %eax

	/* adjust stack registers to point to the now mapped stack as virtual address */
	addl $0xc0000000, %esp
	mov %esp, %ebp

	push %eax
	mov $main_new, %eax
	jmp *%eax
