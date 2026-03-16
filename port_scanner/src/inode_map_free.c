#include <stdlib.h>
#include "inode_map.h"

void	inode_map_free(t_inode_map *map)
{
	t_inode_node	*node;
	t_inode_node	*next;
	size_t			i;

	if (!map)
		return ;
	i = 0;
	while (i < map->capacity)
	{
		node = map->buckets[i];
		while (node)
		{
			next = node->next;
			free(node);
			node = next;
		}
		i++;
	}
	free(map->buckets);
	free(map);
}
