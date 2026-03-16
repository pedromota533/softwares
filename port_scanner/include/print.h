#ifndef PORT_SCANNER_PRINT_H
# define PORT_SCANNER_PRINT_H

# include <stddef.h>
# include <stdint.h>
# include "socket_entry.h"

/* Print a formatted table. If filter_port != 0, only show entries for that port. */
void	print_table(const t_socket_entry *entries, size_t count,
			uint16_t filter_port);

#endif
