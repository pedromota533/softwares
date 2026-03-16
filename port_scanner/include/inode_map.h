#ifndef PORT_SCANNER_INODE_MAP_H
# define PORT_SCANNER_INODE_MAP_H

# include <stdint.h>
# include <stddef.h>
# include <sys/types.h>

typedef struct s_inode_node
{
	uint64_t			inode;
	pid_t				pid;
	struct s_inode_node	*next;
}	t_inode_node;

typedef struct s_inode_map
{
	t_inode_node	**buckets;
	size_t			capacity;
}	t_inode_map;

t_inode_map	*inode_map_create(size_t capacity);
int			inode_map_add(t_inode_map *map, uint64_t inode, pid_t pid);
pid_t		inode_map_lookup(const t_inode_map *map, uint64_t inode);
void		inode_map_free(t_inode_map *map);

#endif
