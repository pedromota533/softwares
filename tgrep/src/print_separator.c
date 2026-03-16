#include <stdio.h>
#include "output.h"
#include "color.h"

void	print_separator(t_config *cfg)
{
	if (cfg->json)
		return ;
	if (cfg->use_color)
		fputs(COL_SEP, stdout);
	fputs("--", stdout);
	if (cfg->use_color)
		fputs(COL_RESET, stdout);
	fputc('\n', stdout);
}
