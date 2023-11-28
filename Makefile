CC		:=	gcc

RM		=	rm -rf

BOOT		=	qemu-system-x86_64

CFLAGS 		+=	-nostdinc -fno-builtin -m32 -fno-pic
CFLAGS		+=	-fno-stack-protector
CFLAGS		+=	-nostdlib -g -Wall
CFLAGS		+=	-I inc/
CFLAGS		+=	-I inc/tools/
CFLAGS		+=	-I inc/tools/kprintf/
CFLAGS		+=	-I inc/tools/kwrite/
CFLAGS		+=	-I src/kmalloc/includes/

LDFLAGS		+=	-nostdlib -nodefaultlibs -m32 -Wl,--build-id=none

ASFLAGS		+=	-m32

NAME		=	mykernel

LD		=	kfs.ld

BOOTSTRAP	=	src/crt0.S		\
			src/isr_handler.S

KMALLOC_SRCS	=	src/kmalloc/mem_infos/mem_infos.c	\
			src/kmalloc/kmalloc/kmalloc.c		\
			src/kmalloc/kmalloc/add_value.c		\
			src/kmalloc/realloc/realloc.c		\
			src/kmalloc/calloc/calloc.c		\
			src/kmalloc/free/free.c			\
			src/kmalloc/free/rm_value.c

SRCS		=	src/main.c				\
			src/io.c				\
			src/vga.c				\
			src/tools.c				\
			src/tools/string.c			\
			src/tools/registers.c			\
			src/segmentation.c			\
			src/interrupts.c			\
			src/irq.c				\
			src/kb.c				\
			src/pit.c				\
			src/uart.c				\
			src/frame.c				\
			src/tools/kprintf/kprintf.c 		\
			src/tools/kprintf/kprintf_tools.c	\
			src/tools/kprintf/put_nbr_base.c 	\
			src/tools/kwrite/kwrite.c		\
			src/pages.c				\
			src/test.c				\
			src/time.c

SRCS		+=	$(KMALLOC_SRCS)

OBJS		=	$(BOOTSTRAP:.S=.o)	\
			$(SRCS:.c=.o)

all:	$(NAME)

$(NAME):$(OBJS)
	$(CC) $(OBJS) -T $(LD) -o $(NAME) $(LDFLAGS)

 .S.o:
	$(CC) -c $(CFLAGS) $< -o $@

boot:
	$(BOOT) -kernel $(NAME) -serial stdio -m 4G -s

debug:
	$(BOOT) -s -S -kernel $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re:	fclean all

.PHONY: all clean fclean re
