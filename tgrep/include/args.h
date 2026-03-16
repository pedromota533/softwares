#ifndef TGREP_ARGS_H
# define TGREP_ARGS_H

# include "tgrep.h"

/* Parse argc/argv into cfg. Returns 0 on success, -1 on error, -2 for --help. */
int		parse_args(int argc, char **argv, t_config *cfg);

/* Print usage information to stderr. */
void	print_usage(const char *prog);

#endif
