#include <unistd.h>
#include "color.h"

void	color_resolve(t_config *cfg)
{
	if (cfg->color_mode == COLOR_ALWAYS)
		cfg->use_color = 1;
	else if (cfg->color_mode == COLOR_NEVER)
		cfg->use_color = 0;
	else
		cfg->use_color = isatty(STDOUT_FILENO);
}
