#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
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
	int			fd;
	int			i;
	time_t		now;
	struct tm	now_tm;
	char		time_buf[64];

	fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd < 0)
		return (0);
	fp = fdopen(fd, "w");
	if (!fp)
	{
		close(fd);
		return (0);
	}
	now = time(NULL);
	if (now == (time_t)-1 || localtime_r(&now, &now_tm) == NULL
		|| strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S %z",
			&now_tm) == 0)
		snprintf(time_buf, sizeof(time_buf), "unknown");
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
	char	**resolved_dirs;
	int		status;
	int		parse_error;
	int		mem_error;

	if (argc < 2)
	{
		print_usage(argv[0]);
		return (1);
	}
	if (!is_valid_workspace_name(argv[1]))
		return (fprintf(stderr,
				"workspace_saver: invalid workspace name (must not be empty and contain only letters, numbers, '_' or '-')\n"), 1);
	if (!resolve_workspace_dir(workspace_dir, sizeof(workspace_dir)))
		return (fprintf(stderr,
				"workspace_saver: unable to prepare $HOME/.config/%s (check HOME and permissions)\n",
				WS_DIR_NAME), 1);
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
	resolved_dirs = calloc((size_t)dir_count, sizeof(*resolved_dirs));
	if (!resolved_dirs)
		return (fprintf(stderr, "workspace_saver: out of memory\n"), 1);
	status = 1;
	parse_error = 0;
	mem_error = 0;
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
		{
			mem_error = 1;
			break ;
		}
		i++;
	}
	if (mem_error)
		fprintf(stderr, "workspace_saver: out of memory\n");
	else if (!parse_error
		&& !write_workspace_file(file_path, dir_count, resolved_dirs))
		fprintf(stderr, "workspace_saver: failed to write %s\n", file_path);
	else if (!parse_error)
		status = 0;
	i = 0;
	while (i < dir_count)
	{
		if (resolved_dirs[i])
			free(resolved_dirs[i]);
		i++;
	}
	free(resolved_dirs);
	if (status != 0)
		return (1);
	printf("Saved workspace '%s' to %s\n", argv[1], file_path);
	return (0);
}
