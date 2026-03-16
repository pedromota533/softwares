#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "proc_pid.h"

#define MAP_CAPACITY 4096

static void	scan_pid_fds(t_inode_map *map, pid_t pid)
{
	char			fd_dir[64];
	char			fd_path[320];
	char			target[128];
	DIR				*dir;
	struct dirent	*entry;
	unsigned long	inode_ul;
	ssize_t			len;

	snprintf(fd_dir, sizeof(fd_dir), "/proc/%d/fd", pid);
	dir = opendir(fd_dir);
	if (!dir)
		return ;
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_name[0] == '.')
			continue ;
		snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd/%s", pid,
			entry->d_name);
		len = readlink(fd_path, target, sizeof(target) - 1);
		if (len < 0)
			continue ;
		target[len] = '\0';
		if (sscanf(target, "socket:[%lu]", &inode_ul) == 1)
			inode_map_add(map, (uint64_t)inode_ul, pid);
	}
	closedir(dir);
}

t_inode_map	*build_inode_map(void)
{
	DIR				*proc_dir;
	struct dirent	*entry;
	t_inode_map		*map;
	char			*endptr;
	long			pid_val;

	map = inode_map_create(MAP_CAPACITY);
	if (!map)
		return (NULL);
	proc_dir = opendir("/proc");
	if (!proc_dir)
	{
		inode_map_free(map);
		return (NULL);
	}
	while ((entry = readdir(proc_dir)) != NULL)
	{
		pid_val = strtol(entry->d_name, &endptr, 10);
		if (*endptr != '\0' || pid_val <= 0)
			continue ;
		scan_pid_fds(map, (pid_t)pid_val);
	}
	closedir(proc_dir);
	return (map);
}
