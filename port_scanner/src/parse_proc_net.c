#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proc_net.h"
#include "utils.h"

#define INITIAL_CAP 64

static int	grow(t_socket_entry **arr, size_t *cap)
{
	size_t			new_cap;
	t_socket_entry	*tmp;

	new_cap = *cap * 2;
	tmp = realloc(*arr, new_cap * sizeof(t_socket_entry));
	if (!tmp)
		return (-1);
	*arr = tmp;
	*cap = new_cap;
	return (0);
}

t_socket_entry	*parse_proc_net(const char *path, t_proto proto,
					size_t *out_count)
{
	FILE			*f;
	char			line[512];
	char			local_addr[64];
	char			rem_addr[64];
	char			ip_hex[9];
	char			port_hex[5];
	unsigned int	state;
	unsigned long	inode;
	t_socket_entry	*arr;
	size_t			cap;
	size_t			count;

	*out_count = 0;
	f = fopen(path, "r");
	if (!f)
		return (NULL);
	arr = malloc(INITIAL_CAP * sizeof(t_socket_entry));
	if (!arr)
	{
		fclose(f);
		return (NULL);
	}
	cap = INITIAL_CAP;
	count = 0;
	if (!fgets(line, sizeof(line), f))
	{
		fclose(f);
		*out_count = 0;
		return (arr);
	}
	while (fgets(line, sizeof(line), f))
	{
		if (sscanf(line, " %*d: %63s %63s %x %*s %*s %*s %*u %*u %lu",
				local_addr, rem_addr, &state, &inode) != 4)
			continue ;
		if (sscanf(local_addr, "%8[^:]:%4s", ip_hex, port_hex) != 2)
			continue ;
		if (count >= cap && grow(&arr, &cap) < 0)
			break ;
		memset(&arr[count], 0, sizeof(t_socket_entry));
		arr[count].proto = proto;
		hex_to_ip(ip_hex, arr[count].local_addr, sizeof(arr[count].local_addr));
		arr[count].port = hex_to_port(port_hex);
		arr[count].state_code = (uint8_t)state;
		arr[count].inode = (uint64_t)inode;
		arr[count].pid = -1;
		strncpy(arr[count].comm, "?", sizeof(arr[count].comm) - 1);
		count++;
	}
	fclose(f);
	*out_count = count;
	return (arr);
}
