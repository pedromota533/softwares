#include <stdio.h>
#include "output.h"

void	print_context_line(const char *line, size_t line_len, long line_num,
			const char *filename, int show_filename, t_config *cfg)
{
	if (cfg->json || cfg->count_only || cfg->files_with || cfg->files_without)
		return ;
	print_prefix(filename, show_filename, line_num, cfg, '-');
	fwrite(line, 1, line_len, stdout);
	fputc('\n', stdout);
}
