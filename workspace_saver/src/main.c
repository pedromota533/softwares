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
#include <curses.h>
#include "workspace_saver.h"

typedef struct s_dirs
{
	char	**items;
	int		count;
	int		capacity;
}	t_dirs;

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
	if (snprintf(config_dir, sizeof(config_dir), "%s/.config",
			home) >= (int)sizeof(config_dir))
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
	char		*resolved;
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

static void	dirs_free(t_dirs *dirs)
{
	int	i;

	i = 0;
	while (i < dirs->count)
	{
		free(dirs->items[i]);
		i++;
	}
	free(dirs->items);
	dirs->items = NULL;
	dirs->count = 0;
	dirs->capacity = 0;
}

static int	dirs_reserve(t_dirs *dirs, int needed)
{
	char	**next;
	int		new_capacity;

	if (needed <= dirs->capacity)
		return (1);
	new_capacity = dirs->capacity;
	if (new_capacity == 0)
		new_capacity = 8;
	while (new_capacity < needed)
		new_capacity *= 2;
	next = realloc(dirs->items, (size_t)new_capacity * sizeof(*next));
	if (!next)
		return (0);
	dirs->items = next;
	dirs->capacity = new_capacity;
	return (1);
}

static int	dirs_add_unique_resolved(t_dirs *dirs, const char *resolved)
{
	char	*copy;
	int		i;

	i = 0;
	while (i < dirs->count)
	{
		if (strcmp(dirs->items[i], resolved) == 0)
			return (1);
		i++;
	}
	if (!dirs_reserve(dirs, dirs->count + 1))
		return (0);
	copy = strdup(resolved);
	if (!copy)
		return (0);
	dirs->items[dirs->count] = copy;
	dirs->count++;
	return (1);
}

static void	dirs_remove_at(t_dirs *dirs, int index)
{
	int	i;

	if (index < 0 || index >= dirs->count)
		return ;
	free(dirs->items[index]);
	i = index;
	while (i < dirs->count - 1)
	{
		dirs->items[i] = dirs->items[i + 1];
		i++;
	}
	dirs->count--;
}

static int	add_directory_from_input(t_dirs *dirs, const char *input)
{
	char	abs_path[PATH_MAX];

	if (!resolve_absolute_dir(input, abs_path, sizeof(abs_path)))
		return (0);
	return (dirs_add_unique_resolved(dirs, abs_path));
}

static int	write_workspace_file(const char *file_path, t_dirs *dirs)
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
		return (close(fd), 0);
	now = time(NULL);
	if (now == (time_t)-1 || localtime_r(&now, &now_tm) == NULL
		|| strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S %z",
			&now_tm) == 0)
		snprintf(time_buf, sizeof(time_buf), "unknown");
	fprintf(fp, "# saved_at=%s\n", time_buf);
	i = 0;
	while (i < dirs->count)
	{
		fprintf(fp, "%s\n", dirs->items[i]);
		i++;
	}
	if (fclose(fp) != 0)
		return (0);
	return (1);
}

static void	print_usage(const char *prog)
{
	fprintf(stderr, "Usage: %s [--ui] <workspace_name> [directory ...]\n", prog);
	fprintf(stderr, "Saves directories to ~/.config/%s/<workspace_name>%s\n",
		WS_DIR_NAME, WS_EXT);
	fprintf(stderr, "Use --ui to manage folders interactively.\n");
}

static int	run_ui(t_dirs *dirs)
{
	char	input[PATH_MAX];
	char	message[256];
	int		selected;
	int		ch;
	int		i;

	selected = 0;
	message[0] = '\0';
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	while (1)
	{
		erase();
		mvprintw(0, 0, "Workspace Folder Manager");
		mvprintw(1, 0, "↑/↓ move  a add  d delete  s save  q quit");
		if (message[0] != '\0')
			mvprintw(2, 0, "%s", message);
		i = 0;
		while (i < dirs->count)
		{
			if (i == selected)
				attron(A_REVERSE);
			mvprintw(4 + i, 0, "%d. %s", i + 1, dirs->items[i]);
			if (i == selected)
				attroff(A_REVERSE);
			i++;
		}
		if (dirs->count == 0)
			mvprintw(4, 0, "(no folders selected)");
		refresh();
		ch = getch();
		if (ch == 'q' || ch == 27)
		{
			endwin();
			return (0);
		}
		if (ch == KEY_UP && selected > 0)
			selected--;
		else if (ch == KEY_DOWN && selected < dirs->count - 1)
			selected++;
		else if (ch == 'd')
		{
			if (dirs->count > 0)
			{
				dirs_remove_at(dirs, selected);
				if (selected >= dirs->count && selected > 0)
					selected--;
				snprintf(message, sizeof(message), "Folder removed.");
			}
		}
		else if (ch == 'a')
		{
			echo();
			curs_set(1);
			mvprintw(LINES - 2, 0, "Add folder path: ");
			clrtoeol();
			getnstr(input, (int)sizeof(input) - 1);
			noecho();
			curs_set(0);
			if (input[0] == '\0')
				snprintf(message, sizeof(message), "No folder added.");
			else if (!add_directory_from_input(dirs, input))
				snprintf(message, sizeof(message), "Invalid folder or memory error.");
			else
			{
				selected = dirs->count - 1;
				snprintf(message, sizeof(message), "Folder added.");
			}
		}
		else if (ch == 's' || ch == '\n')
		{
			if (dirs->count == 0)
				snprintf(message, sizeof(message),
					"Add at least one folder before saving.");
			else
			{
				endwin();
				return (1);
			}
		}
	}
}

int	main(int argc, char **argv)
{
	char	workspace_dir[PATH_MAX];
	char	file_path[PATH_MAX];
	char	cwd[PATH_MAX];
	t_dirs	dirs;
	char	*workspace_name;
	int		use_ui;
	int		i;

	dirs.items = NULL;
	dirs.count = 0;
	dirs.capacity = 0;
	workspace_name = NULL;
	use_ui = 0;
	i = 1;
	while (i < argc)
	{
		if (strcmp(argv[i], "--ui") == 0)
			use_ui = 1;
		else if (!workspace_name)
			workspace_name = argv[i];
		else if (!add_directory_from_input(&dirs, argv[i]))
			return (fprintf(stderr, "workspace_saver: invalid directory: %s\n", argv[i]),
				dirs_free(&dirs), 1);
		i++;
	}
	if (!workspace_name)
		return (print_usage(argv[0]), dirs_free(&dirs), 1);
	if (!is_valid_workspace_name(workspace_name))
		return (fprintf(stderr,
				"workspace_saver: invalid workspace name (must not be empty and contain only letters, numbers, '_' or '-')\n"),
			dirs_free(&dirs), 1);
	if (dirs.count == 0)
	{
		if (!getcwd(cwd, sizeof(cwd)) || !add_directory_from_input(&dirs, cwd))
			return (fprintf(stderr, "workspace_saver: unable to set current directory\n"),
				dirs_free(&dirs), 1);
	}
	if (use_ui && !run_ui(&dirs))
		return (dirs_free(&dirs), 1);
	if (!resolve_workspace_dir(workspace_dir, sizeof(workspace_dir)))
		return (fprintf(stderr,
				"workspace_saver: unable to prepare $HOME/.config/%s (check HOME and permissions)\n",
				WS_DIR_NAME), dirs_free(&dirs), 1);
	if (snprintf(file_path, sizeof(file_path), "%s/%s%s", workspace_dir,
			workspace_name, WS_EXT) >= (int)sizeof(file_path))
		return (fprintf(stderr, "workspace_saver: workspace path too long\n"),
			dirs_free(&dirs), 1);
	if (!write_workspace_file(file_path, &dirs))
		return (fprintf(stderr, "workspace_saver: failed to write %s\n", file_path),
			dirs_free(&dirs), 1);
	printf("Saved workspace '%s' to %s\n", workspace_name, file_path);
	dirs_free(&dirs);
	return (0);
}
