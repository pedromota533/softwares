#include <stdio.h>
#include "print.h"
#include "utils.h"

void	print_table(const t_socket_entry *entries, size_t count,
			uint16_t filter_port)
{
	const t_socket_entry	*e;
	const char				*proto_str;
	size_t					i;

	printf("%-6s %-16s %-6s %-13s %-7s %s\n",
		"Proto", "Local Address", "Port", "State", "PID", "Process");
	printf("%-6s %-16s %-6s %-13s %-7s %s\n",
		"-----", "-------------", "----", "-----", "---", "-------");
	i = 0;
	while (i < count)
	{
		e = &entries[i];
		if (filter_port != 0 && e->port != filter_port)
		{
			i++;
			continue ;
		}
		proto_str = (e->proto == PROTO_TCP) ? "TCP" : "UDP";
		if (e->pid > 0)
			printf("%-6s %-16s %-6u %-13s %-7d <%s>\n",
				proto_str, e->local_addr, e->port,
				state_to_str(e->state_code, e->proto),
				e->pid, e->comm);
		else
			printf("%-6s %-16s %-6u %-13s %-7s <%s>\n",
				proto_str, e->local_addr, e->port,
				state_to_str(e->state_code, e->proto),
				"?", e->comm);
		i++;
	}
}
