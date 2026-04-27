#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "workspace_saver.h"

static int	is_valid_workspace_name(const char *name)
{
	size_t	i;

	if (!name || !*name)
		return (0);
	i = 0;
	while (name[i])
	{
		if (!isalnum((unsigned char)name[i]) && name[i] != '_' && name[i] != '-')
			return (0);
		i++;
	}
	return (1);
}

static int	ensure_dir(const char *path)
{
	struct stat	st;

	if (stat(path, &st) == 0)
		return (S_ISDIR(st.st_mode));
	if (errno != ENOENT)
		return (0);
	return (mkdir(path, 0700) == 0);
}

static int	resolve_workspace_dir(char *out, size_t out_size)
{
	const char	*home;
	char		config_dir[PATH_MAX];

	home = getenv("HOME");
	if (!home || !*home)
		return (0);
	if (snprintf(config_dir, sizeof(config_dir), "%s/.config", home) >= (int)sizeof(config_dir))
		return (0);
	if (!ensure_dir(config_dir))
		return (0);
	if (snprintf(out, out_size, "%s/%s", config_dir, WS_DIR_NAME) >= (int)out_size)
		return (0);
	if (!ensure_dir(out))
		return (0);
	return (1);
}

static int	resolve_absolute_dir(const char *input, char *out, size_t out_size)
{
	char	*resolved;
	struct stat	st;

	resolved = realpath(input, NULL);
	if (!resolved)
		return (0);
	if (stat(resolved, &st) != 0 || !S_ISDIR(st.st_mode))
	{
		free(resolved);
		return (0);
	}
	if (snprintf(out, out_size, "%s", resolved) >= (int)out_size)
	{
		free(resolved);
		return (0);
	}
	free(resolved);
	return (1);
}

static int	write_workspace_file(const char *file_path, int dir_count, char **dirs)
{
	FILE		*fp;
	int			i;
	time_t		now;
	struct tm	now_tm;
	char		time_buf[64];

	fp = fopen(file_path, "w");
	if (!fp)
		return (0);
	now = time(NULL);
	localtime_r(&now, &now_tm);
	strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S %z", &now_tm);
	fprintf(fp, "# saved_at=%s\n", time_buf);
	i = 0;
	while (i < dir_count)
	{
		fprintf(fp, "%s\n", dirs[i]);
		i++;
	}
	if (fclose(fp) != 0)
		return (0);
	return (1);
}

static void	print_usage(const char *prog)
{
	fprintf(stderr, "Usage: %s <workspace_name> [directory ...]\n", prog);
	fprintf(stderr, "Saves directories to ~/.config/%s/<workspace_name>%s\n",
		WS_DIR_NAME, WS_EXT);
}

int	main(int argc, char **argv)
{
	char	workspace_dir[PATH_MAX];
	char	file_path[PATH_MAX];
	char	cwd[PATH_MAX];
	char	abs_path[PATH_MAX];
	int		dir_count;
	int		i;
	char	**inputs;
	char	*single_input[1];
	char	*resolved_dirs[argc > 2 ? argc - 2 : 1];
	int		ok;
	int		parse_error;

	if (argc < 2)
		return (print_usage(argv[0]), 1);
	if (!is_valid_workspace_name(argv[1]))
		return (fprintf(stderr, "workspace_saver: invalid workspace name\n"), 1);
	if (!resolve_workspace_dir(workspace_dir, sizeof(workspace_dir)))
		return (fprintf(stderr, "workspace_saver: unable to prepare config directory\n"), 1);
	if (snprintf(file_path, sizeof(file_path), "%s/%s%s",
			workspace_dir, argv[1], WS_EXT) >= (int)sizeof(file_path))
		return (fprintf(stderr, "workspace_saver: workspace path too long\n"), 1);
	if (argc > 2)
	{
		inputs = &argv[2];
		dir_count = argc - 2;
	}
	else
	{
		if (!getcwd(cwd, sizeof(cwd)))
			return (fprintf(stderr, "workspace_saver: getcwd failed\n"), 1);
		single_input[0] = cwd;
		inputs = single_input;
		dir_count = 1;
	}
	i = 0;
	while (i < dir_count)
	{
		resolved_dirs[i] = NULL;
		i++;
	}
	ok = 0;
	parse_error = 0;
	i = 0;
	while (i < dir_count)
	{
		if (!resolve_absolute_dir(inputs[i], abs_path, sizeof(abs_path)))
		{
			fprintf(stderr, "workspace_saver: invalid directory: %s\n", inputs[i]);
			parse_error = 1;
			break ;
		}
		resolved_dirs[i] = strdup(abs_path);
		if (!resolved_dirs[i])
			break ;
		i++;
	}
	if (parse_error)
		ok = 0;
	else if (i < dir_count && resolved_dirs[i] == NULL)
		fprintf(stderr, "workspace_saver: out of memory\n");
	else if (!write_workspace_file(file_path, dir_count, resolved_dirs))
		fprintf(stderr, "workspace_saver: failed to write %s\n", file_path);
	else
		ok = 1;
	i = 0;
	while (i < dir_count)
	{
		free(resolved_dirs[i]);
		i++;
	}
	if (!ok)
		return (1);
	printf("Saved workspace '%s' to %s\n", argv[1], file_path);
	return (0);
}
