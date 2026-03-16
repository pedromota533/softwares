#include "inode_map.h"

pid_t	inode_map_lookup(const t_inode_map *map, uint64_t inode)
{
	t_inode_node	*node;
	size_t			idx;

	idx = (size_t)(inode % map->capacity);
	node = map->buckets[idx];
	while (node)
	{
		if (node->inode == inode)
			return (node->pid);
		node = node->next;
	}
	return (-1);
}
