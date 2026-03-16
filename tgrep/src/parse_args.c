#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "args.h"
#include "color.h"

void	print_usage(const char *prog)
{
	fprintf(stderr,
		"Usage: %s [OPTIONS] PATTERN [FILE...]\n"
		"       %s [OPTIONS] -e PATTERN [-e PATTERN...] [FILE...]\n"
		"\nMatch options:\n"
		"  -e, --regexp PATTERN      Add a search pattern\n"
		"  -i, --ignore-case         Case insensitive\n"
		"  -S, --smart-case          Insensitive when pattern is all lowercase\n"
		"  -w, --word-regexp         Whole word match\n"
		"  -x, --line-regexp         Whole line match\n"
		"  -v, --invert-match        Show non-matching lines\n"
		"\nOutput options:\n"
		"  -n, --line-number         Show line numbers\n"
		"  -o, --only-matching       Show only the matched part\n"
		"  -c, --count               Count matches per file\n"
		"  -l, --files-with-matches  List files with matches\n"
		"  -L, --files-without-match List files without matches\n"
		"  -H, --with-filename       Always show filename\n"
		"      --no-filename         Never show filename\n"
		"  -Z, --null                NUL-separated output\n"
		"      --color[=WHEN]        Color: auto, always, never (default: auto)\n"
		"      --json                Output as JSON objects\n"
		"      --stats               Print search stats to stderr\n"
		"\nContext options:\n"
		"  -A N, --after-context N   N lines after match\n"
		"  -B N, --before-context N  N lines before match\n"
		"  -C N, --context N         N lines before and after\n"
		"\nFile options:\n"
		"  -r, --recursive           Recursive directory search\n"
		"  -m N, --max-count N       Stop after N matches per file\n"
		"      --include GLOB        Only search files matching GLOB\n"
		"      --exclude-dir DIR     Skip directories named DIR\n"
		"  -s, --no-messages         Suppress error messages\n"
		"  -h, --help                Show this help\n",
		prog, prog);
}

static int	parse_int(const char *name, const char *val, int *out)
{
	char	*end;
	long	n;

	n = strtol(val, &end, 10);
	if (*end != '\0' || n < 0 || n > 1000000)
	{
		fprintf(stderr, "tgrep: invalid value for %s: '%s'\n", name, val);
		return (-1);
	}
	*out = (int)n;
	return (0);
}

static int	add_pattern(t_config *cfg, const char *pat)
{
	if (cfg->pattern_count >= MAX_PATTERNS)
	{
		fprintf(stderr, "tgrep: too many patterns (max %d)\n", MAX_PATTERNS);
		return (-1);
	}
	cfg->patterns[cfg->pattern_count++] = (char *)pat;
	return (0);
}

static const char	*need_val(const char *flag, int *i,
							int argc, char **argv)
{
	if (*i + 1 >= argc)
	{
		fprintf(stderr, "tgrep: %s requires a value\n", flag);
		return (NULL);
	}
	return (argv[++(*i)]);
}

static int	handle_long(t_config *cfg, const char *arg,
				int *i, int argc, char **argv)
{
	const char	*val;
	int			tmp;

	if (!strcmp(arg, "--help"))
		return (-2);
	if (!strcmp(arg, "--ignore-case"))	{ cfg->ignore_case = 1; return (0); }
	if (!strcmp(arg, "--smart-case"))	{ cfg->smart_case = 1; return (0); }
	if (!strcmp(arg, "--word-regexp"))	{ cfg->whole_word = 1; return (0); }
	if (!strcmp(arg, "--line-regexp"))	{ cfg->whole_line = 1; return (0); }
	if (!strcmp(arg, "--invert-match"))	{ cfg->invert = 1; return (0); }
	if (!strcmp(arg, "--line-number"))	{ cfg->line_numbers = 1; return (0); }
	if (!strcmp(arg, "--only-matching")){ cfg->only_matching = 1; return (0); }
	if (!strcmp(arg, "--count"))		{ cfg->count_only = 1; return (0); }
	if (!strcmp(arg, "--files-with-matches")) { cfg->files_with = 1; return (0); }
	if (!strcmp(arg, "--files-without-match")){ cfg->files_without = 1; return (0); }
	if (!strcmp(arg, "--with-filename"))	{ cfg->with_filename = 1; return (0); }
	if (!strcmp(arg, "--no-filename"))	{ cfg->with_filename = -1; return (0); }
	if (!strcmp(arg, "--null"))			{ cfg->null_sep = 1; return (0); }
	if (!strcmp(arg, "--recursive"))	{ cfg->recursive = 1; return (0); }
	if (!strcmp(arg, "--no-messages"))	{ cfg->silent = 1; return (0); }
	if (!strcmp(arg, "--json"))			{ cfg->json = 1; return (0); }
	if (!strcmp(arg, "--stats"))		{ cfg->stats = 1; return (0); }
	if (!strncmp(arg, "--color", 7))
	{
		val = (arg[7] == '=') ? arg + 8 : "auto";
		if (!strcmp(val, "always") || !strcmp(val, "yes"))
			cfg->color_mode = COLOR_ALWAYS;
		else if (!strcmp(val, "never") || !strcmp(val, "no"))
			cfg->color_mode = COLOR_NEVER;
		else
			cfg->color_mode = COLOR_AUTO;
		return (0);
	}
	if (!strcmp(arg, "--regexp") || !strcmp(arg, "-e"))
	{
		val = need_val(arg, i, argc, argv);
		return (val ? add_pattern(cfg, val) : -1);
	}
	if (!strcmp(arg, "--after-context"))
	{
		val = need_val(arg, i, argc, argv);
		return (val ? parse_int("--after-context", val, &cfg->after) : -1);
	}
	if (!strcmp(arg, "--before-context"))
	{
		val = need_val(arg, i, argc, argv);
		return (val ? parse_int("--before-context", val, &cfg->before) : -1);
	}
	if (!strcmp(arg, "--context"))
	{
		val = need_val(arg, i, argc, argv);
		if (!val)
			return (-1);
		if (parse_int("--context", val, &tmp) < 0)
			return (-1);
		cfg->before = tmp;
		cfg->after = tmp;
		return (0);
	}
	if (!strcmp(arg, "--max-count"))
	{
		val = need_val(arg, i, argc, argv);
		return (val ? parse_int("--max-count", val, &cfg->max_count) : -1);
	}
	if (!strcmp(arg, "--include"))
	{
		val = need_val(arg, i, argc, argv);
		if (!val || cfg->include_count >= MAX_INCLUDES)
			return (-1);
		cfg->include_globs[cfg->include_count++] = (char *)val;
		return (0);
	}
	if (!strcmp(arg, "--exclude-dir"))
	{
		val = need_val(arg, i, argc, argv);
		if (!val || cfg->exclude_dir_count >= MAX_EXCLUDES)
			return (-1);
		cfg->exclude_dirs[cfg->exclude_dir_count++] = (char *)val;
		return (0);
	}
	fprintf(stderr, "tgrep: unknown option '%s'\n", arg);
	return (-1);
}

static int	handle_short_val(t_config *cfg, char flag, const char *val)
{
	int	tmp;

	if (flag == 'e')
		return (add_pattern(cfg, val));
	if (flag == 'A')
		return (parse_int("-A", val, &cfg->after));
	if (flag == 'B')
		return (parse_int("-B", val, &cfg->before));
	if (flag == 'm')
		return (parse_int("-m", val, &cfg->max_count));
	if (flag == 'C')
	{
		if (parse_int("-C", val, &tmp) < 0)
			return (-1);
		cfg->before = tmp;
		cfg->after = tmp;
		return (0);
	}
	return (-1);
}

static int	handle_short(t_config *cfg, const char *arg,
				int *i, int argc, char **argv)
{
	int		j;
	char	f;

	j = 1;
	while (arg[j])
	{
		f = arg[j];
		if (f == 'h')
			return (-2);
		else if (f == 'i') cfg->ignore_case = 1;
		else if (f == 'S') cfg->smart_case = 1;
		else if (f == 'w') cfg->whole_word = 1;
		else if (f == 'x') cfg->whole_line = 1;
		else if (f == 'v') cfg->invert = 1;
		else if (f == 'n') cfg->line_numbers = 1;
		else if (f == 'o') cfg->only_matching = 1;
		else if (f == 'c') cfg->count_only = 1;
		else if (f == 'l') cfg->files_with = 1;
		else if (f == 'L') cfg->files_without = 1;
		else if (f == 'H') cfg->with_filename = 1;
		else if (f == 'Z') cfg->null_sep = 1;
		else if (f == 'r') cfg->recursive = 1;
		else if (f == 's') cfg->silent = 1;
		else if (f == 'e' || f == 'A' || f == 'B' || f == 'C' || f == 'm')
		{
			const char *val = arg[j + 1] ? arg + j + 1
				: (*i + 1 < argc ? argv[++(*i)] : NULL);
			if (!val)
			{
				fprintf(stderr, "tgrep: -%c requires a value\n", f);
				return (-1);
			}
			if (handle_short_val(cfg, f, val) < 0)
				return (-1);
			break ;
		}
		else
		{
			fprintf(stderr, "tgrep: unknown option '-%c'\n", f);
			return (-1);
		}
		j++;
	}
	return (0);
}

int	parse_args(int argc, char **argv, t_config *cfg)
{
	char	**pos;
	int		pos_count;
	int		end_opts;
	int		ret;
	int		i;
	int		start;

	pos = malloc(argc * sizeof(char *));
	if (!pos)
		return (-1);
	pos_count = 0;
	end_opts = 0;
	i = 1;
	while (i < argc)
	{
		if (!end_opts && !strcmp(argv[i], "--"))
		{
			end_opts = 1;
			i++;
			continue ;
		}
		if (!end_opts && argv[i][0] == '-' && argv[i][1])
		{
			ret = (argv[i][1] == '-')
				? handle_long(cfg, argv[i], &i, argc, argv)
				: handle_short(cfg, argv[i], &i, argc, argv);
			if (ret == -2)
			{
				print_usage(argv[0]);
				free(pos);
				return (-2);
			}
			if (ret < 0)
			{
				free(pos);
				return (-1);
			}
		}
		else
			pos[pos_count++] = argv[i];
		i++;
	}
	start = 0;
	if (cfg->pattern_count == 0)
	{
		if (pos_count == 0)
		{
			print_usage(argv[0]);
			free(pos);
			return (-1);
		}
		add_pattern(cfg, pos[0]);
		start = 1;
	}
	cfg->file_count = pos_count - start;
	if (cfg->file_count > 0)
	{
		cfg->files = malloc(cfg->file_count * sizeof(char *));
		if (!cfg->files)
		{
			free(pos);
			return (-1);
		}
		for (i = 0; i < cfg->file_count; i++)
			cfg->files[i] = pos[start + i];
	}
	free(pos);
	return (0);
}
