#include <stdio.h>
#include "search.h"

int	is_binary(const char *filename)
{
	FILE			*f;
	unsigned char	buf[8192];
	size_t			n;
	size_t			i;

	if (!filename)
		return (0);
	f = fopen(filename, "rb");
	if (!f)
		return (0);
	n = fread(buf, 1, sizeof(buf), f);
	fclose(f);
	i = 0;
	while (i < n)
	{
		if (buf[i] == 0)
			return (1);
		i++;
	}
	return (0);
}
