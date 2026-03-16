#ifndef PORT_SCANNER_ARGS_H
# define PORT_SCANNER_ARGS_H

# include <stdint.h>

/* Parsed command-line arguments. filter_port == 0 means show all ports. */
typedef struct s_args
{
	uint16_t	filter_port;
}	t_args;

/* Parse argc/argv into t_args. Returns 0 on success, -1 on error. */
int	parse_args(int argc, char **argv, t_args *out);

#endif
