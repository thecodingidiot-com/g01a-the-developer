NAME    = game
CC      = gcc
CFLAGS  = -Wall -Wextra -g -std=c99
SRCS    = main.c load.c display.c game.c
OBJS    = $(SRCS:.c=.o)
LIBS    = libtci.a libtciutil.a

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(NAME)

%.o: %.c game.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
