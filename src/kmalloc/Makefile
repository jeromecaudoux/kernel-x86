##
## Makefile for kmalloc in /home/caudou_j/Documents/projets/PSU_2014_kmalloc
## 
## Made by caudou_j
## Login   <caudou_j@epitech.net>
## 
## Started on  Tue Feb  4 12:47:47 2014 caudou_j
## Last update Fri Feb  7 16:02:57 2014 caudou_j
##

NAME		=	libmy_kmalloc_$(HOSTTYPE).so
#NAME		=	m

SRCS		=	mem_infos/mem_infos.c		\
			kmalloc/kmalloc.c			\
			kmalloc/add_value.c		\
			realloc/realloc.c		\
			calloc/calloc.c			\
			free/free.c			\
			free/rm_value.c

CC		= 	gcc

CFLAGS		=	-pedantic -Wall -Wextra
CFLAGS		+=	-Iincludes/
CFLAGS		+=	-Ikmalloc_list/
CFLAGS		+=	-fPIC

OBJS		=	$(SRCS:.c=.o)

RM		=	rm -rf

all		:	$(NAME)

$(NAME)		:	$(OBJS)
			$(CC) -shared -o $(NAME) $(OBJS)

clean		:
			$(RM) $(OBJS)

fclean		:	clean
			$(RM) $(NAME)

re		:	fclean all

.PHONY		:	all clean fclean re
