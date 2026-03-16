#include <fnmatch.h>
#include <string.h>
#include "search.h"

int	glob_match(const char *filename, t_config *cfg)
{
	const char	*base;
	int			i;

	if (cfg->include_count == 0)
		return (1);
	base = strrchr(filename, '/');
	base = base ? base + 1 : filename;
	i = 0;
	while (i < cfg->include_count)
	{
		if (fnmatch(cfg->include_globs[i], base, 0) == 0)
			return (1);
		i++;
	}
	return (0);
}
