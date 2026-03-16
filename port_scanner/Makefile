NAME	= port_scanner
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -I include
RFLAGS	= -O2 -DNDEBUG
DFLAGS	= -g3 -O0 -DDEBUG
PREFIX	= /usr/local/bin

SRCS	= src/main.c \
		  src/parse_args.c \
		  src/parse_proc_net.c \
		  src/build_inode_map.c \
		  src/resolve_pid_name.c \
		  src/inode_map_create.c \
		  src/inode_map_add.c \
		  src/inode_map_lookup.c \
		  src/inode_map_free.c \
		  src/hex_to_ip.c \
		  src/hex_to_port.c \
		  src/state_to_str.c \
		  src/print_table.c

ROBJS	= $(SRCS:src/%.c=obj/release/%.o)
DOBJS	= $(SRCS:src/%.c=obj/debug/%.o)

all: release

release: $(ROBJS)
	$(CC) $(CFLAGS) $(RFLAGS) -o $(NAME) $(ROBJS)

debug: $(DOBJS)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(NAME)_debug $(DOBJS)

obj/release/%.o: src/%.c | obj/release
	$(CC) $(CFLAGS) $(RFLAGS) -c $< -o $@

obj/debug/%.o: src/%.c | obj/debug
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

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
