#include <stdio.h>
#include "output.h"
#include "color.h"

void	print_prefix(const char *filename, int show_filename, long line_num,
			t_config *cfg, char sep)
{
	if (show_filename && filename)
	{
		if (cfg->use_color)
			fputs(COL_FILENAME, stdout);
		fputs(filename, stdout);
		if (cfg->use_color)
			fputs(COL_RESET, stdout);
		fputc(sep, stdout);
	}
	if (cfg->line_numbers)
	{
		if (cfg->use_color)
			fputs(COL_LINENO, stdout);
		printf("%ld", line_num);
		if (cfg->use_color)
			fputs(COL_RESET, stdout);
		fputc(sep, stdout);
	}
}
