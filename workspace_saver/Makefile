NAME	= workspace_saver
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -I include
RFLAGS	= -O2 -DNDEBUG
DFLAGS	= -g3 -O0 -DDEBUG
PREFIX	= /usr/local/bin
NCURSES_CFLAGS := $(shell pkg-config --cflags ncursesw 2>/dev/null || pkg-config --cflags ncurses 2>/dev/null)
NCURSES_LIBS := $(shell pkg-config --libs ncursesw 2>/dev/null || pkg-config --libs ncurses 2>/dev/null || echo -lncurses)

SRCS	= src/main.c

ROBJS	= $(SRCS:src/%.c=obj/release/%.o)
DOBJS	= $(SRCS:src/%.c=obj/debug/%.o)

all: release

release: $(ROBJS)
	$(CC) $(CFLAGS) $(NCURSES_CFLAGS) $(RFLAGS) -o $(NAME) $(ROBJS) $(NCURSES_LIBS)

debug: $(DOBJS)
	$(CC) $(CFLAGS) $(NCURSES_CFLAGS) $(DFLAGS) -o $(NAME)_debug $(DOBJS) $(NCURSES_LIBS)

obj/release/%.o: src/%.c | obj/release
	$(CC) $(CFLAGS) $(NCURSES_CFLAGS) $(RFLAGS) -c $< -o $@

obj/debug/%.o: src/%.c | obj/debug
	$(CC) $(CFLAGS) $(NCURSES_CFLAGS) $(DFLAGS) -c $< -o $@

obj/release obj/debug:
	mkdir -p $@

install: release
	install -m 755 $(NAME) $(PREFIX)/$(NAME)

uninstall:
	rm -f $(PREFIX)/$(NAME)

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME) $(NAME)_debug

re: fclean all

.PHONY: all release debug install uninstall clean fclean re
