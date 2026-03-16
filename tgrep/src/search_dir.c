#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include "search.h"

static int	is_excluded_dir(const char *name, t_config *cfg)
{
	int	i;

	i = 0;
	while (i < cfg->exclude_dir_count)
	{
		if (fnmatch(cfg->exclude_dirs[i], name, 0) == 0)
			return (1);
		i++;
	}
	return (0);
}

static char	*build_path(const char *dir, const char *name)
{
	size_t	dlen;
	size_t	nlen;
	char	*path;

	dlen = strlen(dir);
	nlen = strlen(name);
	path = malloc(dlen + nlen + 2);
	if (!path)
		return (NULL);
	memcpy(path, dir, dlen);
	path[dlen] = '/';
	memcpy(path + dlen + 1, name, nlen);
	path[dlen + 1 + nlen] = '\0';
	return (path);
}

long	search_dir(const char *path, t_matcher *matcher, t_config *cfg)
{
	DIR				*dir;
	struct dirent	*entry;
	struct stat		st;
	char			*fullpath;
	long			total;

	total = 0;
	dir = opendir(path);
	if (!dir)
	{
		if (!cfg->silent)
			fprintf(stderr, "tgrep: %s: cannot open directory\n", path);
		return (0);
	}
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_name[0] == '.')
			continue ;
		if (is_excluded_dir(entry->d_name, cfg))
			continue ;
		fullpath = build_path(path, entry->d_name);
		if (!fullpath)
			continue ;
		if (lstat(fullpath, &st) < 0)
		{
			free(fullpath);
			continue ;
		}
		if (S_ISLNK(st.st_mode))
		{
			free(fullpath);
			continue ;
		}
		if (S_ISDIR(st.st_mode))
			total += search_dir(fullpath, matcher, cfg);
		else if (S_ISREG(st.st_mode))
		{
			if (glob_match(entry->d_name, cfg) && !is_binary(fullpath))
				total += search_file(fullpath, 1, matcher, cfg);
		}
		free(fullpath);
	}
	closedir(dir);
	return (total);
}
