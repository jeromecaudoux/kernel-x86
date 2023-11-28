# define isr_handler \
	pushl %ds; \
	pushl %es; \
	pushl %fs; \
	pushl %gs; \
	pushl %ss; \
	pushal; \
	pushl %esp; \
	call irq_handler; \
	popl %esp; \
	popal; \
	popl %ss; \
	popl %gs; \
	popl %fs; \
	popl %es; \
	popl %ds;
	
#define CONCAT(A, B) A ## B

#define ISR_NO_ERROR(N)			\
	.global CONCAT(isr, N); 	\
	CONCAT(isr, N) : 		\
		pushl $0;		\
		pushl $N;		\
		isr_handler		\
		add $8, %esp;		\
		iret;

#define ISR_WITH_ERROR(N)	 	\
	.global CONCAT(isr, N); 	\
	CONCAT(isr, N) : 		\
		pushl $N;		\
		isr_handler		\
		add $4, %esp;		\
		iret;
ISR_NO_ERROR(0)
ISR_NO_ERROR(1)
ISR_NO_ERROR(2)
ISR_NO_ERROR(3)
ISR_NO_ERROR(4)
ISR_NO_ERROR(5)
ISR_NO_ERROR(6)
ISR_NO_ERROR(7)
ISR_WITH_ERROR(8)
ISR_NO_ERROR(9)
ISR_WITH_ERROR(10)
ISR_WITH_ERROR(11)
ISR_WITH_ERROR(12)
ISR_WITH_ERROR(13)
ISR_WITH_ERROR(14)

ISR_NO_ERROR(16)
ISR_NO_ERROR(17)
ISR_NO_ERROR(18)
ISR_NO_ERROR(19)
ISR_NO_ERROR(20)

ISR_NO_ERROR(30)
ISR_NO_ERROR(31)

ISR_NO_ERROR(32) // Programmable Interrupt Timer Interrupt
ISR_NO_ERROR(33) // Keyboard Interrupt
ISR_NO_ERROR(36) // uart com1 and com3
