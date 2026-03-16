#include <stdlib.h>
#include "inode_map.h"

t_inode_map	*inode_map_create(size_t capacity)
{
	t_inode_map	*map;

	map = malloc(sizeof(t_inode_map));
	if (!map)
		return (NULL);
	map->buckets = calloc(capacity, sizeof(t_inode_node *));
	if (!map->buckets)
	{
		free(map);
		return (NULL);
	}
	map->capacity = capacity;
	return (map);
}
