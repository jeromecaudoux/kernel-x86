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
	/* https://en.wikipedia.org/wiki/Control_register */
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

.global switch_to_userland
.extern postulmain_userland
switch_to_userland:
	cli
	movw $0x23, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs #SS is handled by iret
	
	#push %eax
	#mov %cr0, %eax
	#or $0x00000001, %eax
	#mov %eax, %cr0
	#pop %eax

	mov %esp, %eax
	pushl $0x23        # New stack segment selector (RPL 3)
	pushl %eax  	# New stack pointer

	pushf              # Push flags with "or $0x200" to re-enable interrupts in user mode
	pop %eax
	or $0x200, %eax
	push %eax

	pushl $0x1B        # New code segment (RPL 3)
	pushl $hi   	# Address to jump to
	iret               # Jump to user mode safely

hi:
	push %eax
	mov $main_userland, %eax
	call *%eax

.global tss_flush
tss_flush:
   movw $0x2B, %ax      # Load the index of our TSS structure - The index is
                     # 0x28, as it is the 5th selector and each is 8 bytes
                     # long, but we set the bottom two bits (making 0x2B)
                     # so that it has an RPL of 3, not zero.
   ltr %ax            # Load 0x2B into the task state register.
   ret

.global gdt_flush
gdt_flush:
   movl 4(%esp), %eax
   lgdt (%eax)

   movw $0x10, %ax
   movw %ax, %ds
   movw %ax, %es
   movw %ax, %fs
   movw %ax, %gs
   movw %ax, %ss

   ljmp $0x08, $.flush
.flush:
   ret