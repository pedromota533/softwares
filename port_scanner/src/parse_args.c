#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "args.h"

int	parse_args(int argc, char **argv, t_args *out)
{
	long	port;
	char	*endptr;
	char	*value;
	int		i;

	out->filter_port = 0;
	i = 1;
	while (i < argc)
	{
		if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0)
		{
			if (i + 1 >= argc)
			{
				fprintf(stderr, "port_scanner: %s requires a port number\n",
					argv[i]);
				return (-1);
			}
			value = argv[i + 1];
			port = strtol(value, &endptr, 10);
			if (*endptr != '\0' || port <= 0 || port > 65535)
			{
				fprintf(stderr,
					"port_scanner: invalid port '%s' (must be 1-65535)\n",
					value);
				return (-1);
			}
			out->filter_port = (uint16_t)port;
			i += 2;
		}
		else
		{
			fprintf(stderr, "port_scanner: unknown option '%s'\n", argv[i]);
			fprintf(stderr, "Usage: port_scanner [-p|--port <port>]\n");
			return (-1);
		}
	}
	return (0);
}
