#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "tgrep.h"
#include "args.h"
#include "color.h"
#include "matcher.h"
#include "search.h"
#include "output.h"

static int	resolve_show_filename(t_config *cfg)
{
	if (cfg->with_filename == 1)
		return (1);
	if (cfg->with_filename == -1)
		return (0);
	return (cfg->file_count > 1 || cfg->recursive);
}

static long	run_search(t_config *cfg, t_matcher *matcher)
{
	long	total;
	int		show_fn;
	int		i;

	total = 0;
	show_fn = resolve_show_filename(cfg);
	if (cfg->file_count == 0)
	{
		total = search_file(NULL, 0, matcher, cfg);
		return (total);
	}
	i = 0;
	while (i < cfg->file_count)
	{
		if (cfg->recursive)
			total += search_dir(cfg->files[i], matcher, cfg);
		else
		{
			if (!is_binary(cfg->files[i]))
				total += search_file(cfg->files[i], show_fn, matcher, cfg);
			else if (!cfg->silent)
				fprintf(stderr, "tgrep: %s: binary file skipped\n",
					cfg->files[i]);
		}
		i++;
	}
	return (total);
}

int	main(int argc, char **argv)
{
	t_config	cfg;
	t_matcher	*matcher;
	long		total;
	int			ret;

	signal(SIGPIPE, SIG_IGN);
	memset(&cfg, 0, sizeof(cfg));
	cfg.color_mode = COLOR_AUTO;
	ret = parse_args(argc, argv, &cfg);
	if (ret == -2)
	{
		free(cfg.files);
		return (0);
	}
	if (ret < 0)
	{
		free(cfg.files);
		return (2);
	}
	color_resolve(&cfg);
	matcher = matcher_create(&cfg);
	if (!matcher)
	{
		free(cfg.files);
		return (2);
	}
	total = run_search(&cfg, matcher);
	if (cfg.stats)
		print_stats(&cfg);
	matcher_free(matcher);
	free(cfg.files);
	return (total > 0 ? 0 : 1);
}
