#include "utils.h"

const char	*state_to_str(uint8_t state_code, t_proto proto)
{
	static const char	*tcp_states[] = {
		"UNKNOWN",
		"ESTABLISHED",
		"SYN_SENT",
		"SYN_RECV",
		"FIN_WAIT1",
		"FIN_WAIT2",
		"TIME_WAIT",
		"CLOSE",
		"CLOSE_WAIT",
		"LAST_ACK",
		"LISTEN",
		"CLOSING"
	};

	if (proto == PROTO_UDP)
		return ("-");
	if (state_code >= sizeof(tcp_states) / sizeof(tcp_states[0]))
		return ("UNKNOWN");
	return (tcp_states[state_code]);
}
