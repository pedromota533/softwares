#include <stdlib.h>
#include "inode_map.h"

int	inode_map_add(t_inode_map *map, uint64_t inode, pid_t pid)
{
	t_inode_node	*node;
	size_t			idx;

	node = malloc(sizeof(t_inode_node));
	if (!node)
		return (-1);
	node->inode = inode;
	node->pid = pid;
	idx = (size_t)(inode % map->capacity);
	node->next = map->buckets[idx];
	map->buckets[idx] = node;
	return (0);
}
