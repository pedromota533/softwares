#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "args.h"
#include "proc_net.h"
#include "proc_pid.h"
#include "print.h"

int	main(int argc, char **argv)
{
	t_args			args;
	t_socket_entry	*tcp_entries;
	t_socket_entry	*udp_entries;
	t_inode_map		*map;
	size_t			tcp_count;
	size_t			udp_count;

	signal(SIGPIPE, SIG_IGN);
	if (parse_args(argc, argv, &args) < 0)
		return (1);
	tcp_entries = parse_proc_net("/proc/net/tcp", PROTO_TCP, &tcp_count);
	udp_entries = parse_proc_net("/proc/net/udp", PROTO_UDP, &udp_count);
	map = build_inode_map();
	if (!map)
	{
		fprintf(stderr, "port_scanner: failed to build inode map\n");
		free(tcp_entries);
		free(udp_entries);
		return (1);
	}
	if (tcp_entries)
		resolve_pid_name(tcp_entries, tcp_count, map);
	if (udp_entries)
		resolve_pid_name(udp_entries, udp_count, map);
	inode_map_free(map);
	if (tcp_entries)
		print_table(tcp_entries, tcp_count, args.filter_port);
	if (udp_entries)
		print_table(udp_entries, udp_count, args.filter_port);
	free(tcp_entries);
	free(udp_entries);
	return (0);
}
