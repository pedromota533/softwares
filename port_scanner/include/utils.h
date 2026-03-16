#ifndef PORT_SCANNER_UTILS_H
# define PORT_SCANNER_UTILS_H

# include <stdint.h>
# include <stddef.h>
# include "socket_entry.h"

void		hex_to_ip(const char *hex8, char *out, size_t out_size);
uint16_t	hex_to_port(const char *hex4);
const char	*state_to_str(uint8_t state_code, t_proto proto);

#endif
