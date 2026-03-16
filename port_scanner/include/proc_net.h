#ifndef PORT_SCANNER_PROC_NET_H
# define PORT_SCANNER_PROC_NET_H

# include <stddef.h>
# include "socket_entry.h"

t_socket_entry	*parse_proc_net(const char *path, t_proto proto,
					size_t *out_count);

#endif
