NAME	= tgrep
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -I include -DPCRE2_CODE_UNIT_WIDTH=8
RFLAGS	= -O2 -DNDEBUG
DFLAGS	= -g3 -O0 -DDEBUG
PREFIX	= /usr/local/bin

PCRE2_LIBS	:= $(shell pcre2-config --libs8 2>/dev/null)
ifeq ($(PCRE2_LIBS),)
$(error PCRE2 not found. Install with: sudo apt install libpcre2-dev)
endif

SRCS	= src/main.c \
		  src/parse_args.c \
		  src/color_resolve.c \
		  src/matcher_create.c \
		  src/matcher_free.c \
		  src/match_line.c \
		  src/context_init.c \
		  src/context_push.c \
		  src/context_drain.c \
		  src/context_free.c \
		  src/print_prefix.c \
		  src/print_match.c \
		  src/print_context_line.c \
		  src/print_separator.c \
		  src/print_stats.c \
		  src/search_file.c \
		  src/search_dir.c \
		  src/is_binary.c \
		  src/glob_match.c

ROBJS	= $(SRCS:src/%.c=obj/release/%.o)
DOBJS	= $(SRCS:src/%.c=obj/debug/%.o)

all: release

release: $(ROBJS)
	$(CC) $(CFLAGS) $(RFLAGS) -o $(NAME) $(ROBJS) $(PCRE2_LIBS)

debug: $(DOBJS)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(NAME)_debug $(DOBJS) $(PCRE2_LIBS)

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
