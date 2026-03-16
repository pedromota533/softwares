#include <stdio.h>
#include <string.h>
#include "proc_pid.h"

void	resolve_pid_name(t_socket_entry *entries, size_t count,
			t_inode_map *map)
{
	FILE		*f;
	char		comm_path[64];
	char		*nl;
	size_t		i;

	i = 0;
	while (i < count)
	{
		if (entries[i].inode == 0)
		{
			i++;
			continue ;
		}
		entries[i].pid = inode_map_lookup(map, entries[i].inode);
		if (entries[i].pid <= 0)
		{
			strncpy(entries[i].comm, "?", sizeof(entries[i].comm) - 1);
			i++;
			continue ;
		}
		snprintf(comm_path, sizeof(comm_path), "/proc/%d/comm",
			entries[i].pid);
		f = fopen(comm_path, "r");
		if (!f)
		{
			strncpy(entries[i].comm, "?", sizeof(entries[i].comm) - 1);
			i++;
			continue ;
		}
		if (!fgets(entries[i].comm, sizeof(entries[i].comm), f))
			strncpy(entries[i].comm, "?", sizeof(entries[i].comm) - 1);
		nl = strchr(entries[i].comm, '\n');
		if (nl)
			*nl = '\0';
		fclose(f);
		i++;
	}
}
