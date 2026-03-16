#include <stdlib.h>
#include "matcher.h"

void	matcher_free(t_matcher *m)
{
	int	i;

	if (!m)
		return ;
	i = 0;
	while (i < m->count)
	{
		if (m->codes[i])
			pcre2_code_free(m->codes[i]);
		i++;
	}
	free(m->codes);
	free(m);
}
