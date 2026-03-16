#include <stdio.h>
#include "output.h"

void	print_stats(t_config *cfg)
{
	fprintf(stderr, "\n-- tgrep stats --\n");
	fprintf(stderr, "Files searched : %ld\n", cfg->stat_files_searched);
	fprintf(stderr, "Files matched  : %ld\n", cfg->stat_files_matched);
	fprintf(stderr, "Total matches  : %ld\n", cfg->stat_total_matches);
}
