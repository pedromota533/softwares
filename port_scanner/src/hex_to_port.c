#include <stdlib.h>
#include "utils.h"

uint16_t	hex_to_port(const char *hex4)
{
	return ((uint16_t)strtoul(hex4, NULL, 16));
}
