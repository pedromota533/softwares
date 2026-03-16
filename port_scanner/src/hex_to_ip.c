#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void	hex_to_ip(const char *hex8, char *out, size_t out_size)
{
	uint32_t		raw;
	unsigned char	*b;

	raw = (uint32_t)strtoul(hex8, NULL, 16);
	b = (unsigned char *)&raw;
	snprintf(out, out_size, "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);
}
