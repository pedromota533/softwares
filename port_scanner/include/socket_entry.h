#ifndef PORT_SCANNER_SOCKET_ENTRY_H
# define PORT_SCANNER_SOCKET_ENTRY_H

# include <stdint.h>
# include <sys/types.h>

typedef enum e_proto
{
	PROTO_TCP,
	PROTO_UDP
}	t_proto;

typedef struct s_socket_entry
{
	t_proto		proto;
	char		local_addr[16];
	uint16_t	port;
	uint8_t		state_code;
	uint64_t	inode;
	pid_t		pid;
	char		comm[17];
}	t_socket_entry;

#endif
