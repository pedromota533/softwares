#ifndef PORT_SCANNER_PROC_PID_H
# define PORT_SCANNER_PROC_PID_H

# include <stddef.h>
# include "inode_map.h"
# include "socket_entry.h"

t_inode_map	*build_inode_map(void);
void		resolve_pid_name(t_socket_entry *entries, size_t count,
				t_inode_map *map);

#endif
