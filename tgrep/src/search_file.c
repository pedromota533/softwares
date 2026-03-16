#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"
#include "output.h"
#include "context.h"

typedef struct s_fstate
{
	t_config	*cfg;
	t_matcher	*matcher;
	const char	*filename;
	int			show_filename;
	t_context	*bctx;
	int			after_rem;
	long		last_printed;
	long		match_count;
}	t_fstate;

static void	maybe_separator(t_fstate *st, long line_num)
{
	int	has_ctx;

	has_ctx = (st->cfg->before > 0 || st->cfg->after > 0);
	if (has_ctx && st->last_printed > 0 && line_num > st->last_printed + 1)
		print_separator(st->cfg);
}

static void	handle_match(t_fstate *st, const char *line, size_t len,
				long line_num, t_match *matches, int mc)
{
	if (!st->cfg->count_only && !st->cfg->files_with
		&& !st->cfg->files_without)
	{
		maybe_separator(st, st->bctx && st->bctx->size > 0
			? st->bctx->buf[st->bctx->head].line_num : line_num);
		if (st->bctx)
			context_drain(st->bctx, st->filename, st->show_filename,
				st->cfg, &st->last_printed);
		maybe_separator(st, line_num);
		print_match(line, len, line_num, matches, mc,
			st->filename, st->show_filename, st->cfg);
		st->last_printed = line_num;
	}
	st->after_rem = st->cfg->after;
	st->match_count++;
}

static void	handle_context_after(t_fstate *st, const char *line, size_t len,
				long line_num)
{
	print_context_line(line, len, line_num, st->filename,
		st->show_filename, st->cfg);
	st->last_printed = line_num;
	st->after_rem--;
}

long	search_file(const char *filename, int show_filename,
			t_matcher *matcher, t_config *cfg)
{
	FILE		*f;
	char		*line;
	size_t		cap;
	ssize_t		raw_len;
	size_t		len;
	long		line_num;
	t_fstate	st;
	t_match		*matches;
	int			mc;
	int			has_match;
	char		end_sep;

	if (!filename)
		f = stdin;
	else
	{
		f = fopen(filename, "r");
		if (!f)
		{
			if (!cfg->silent)
				fprintf(stderr, "tgrep: %s: cannot open file\n", filename);
			return (0);
		}
	}
	memset(&st, 0, sizeof(st));
	st.cfg = cfg;
	st.matcher = matcher;
	st.filename = filename ? filename : "(stdin)";
	st.show_filename = show_filename;
	if (cfg->before > 0)
		st.bctx = context_init(cfg->before);
	line = NULL;
	cap = 0;
	line_num = 0;
	while ((raw_len = getline(&line, &cap, f)) != -1)
	{
		line_num++;
		len = (size_t)raw_len;
		if (len > 0 && line[len - 1] == '\n')
			len--;
		if (len > 0 && line[len - 1] == '\r')
			len--;
		mc = 0;
		matches = match_line(matcher, line, len, &mc);
		has_match = (mc > 0);
		if (cfg->invert ? !has_match : has_match)
		{
			handle_match(&st, line, len, line_num,
				cfg->invert ? NULL : matches, cfg->invert ? 0 : mc);
			if (cfg->max_count > 0 && st.match_count >= cfg->max_count)
			{
				free(matches);
				break ;
			}
		}
		else if (st.after_rem > 0)
			handle_context_after(&st, line, len, line_num);
		else if (st.bctx)
			context_push(st.bctx, line, len, line_num);
		free(matches);
	}
	free(line);
	context_free(st.bctx);
	if (filename)
		fclose(f);
	cfg->stat_files_searched++;
	if (st.match_count > 0)
		cfg->stat_files_matched++;
	cfg->stat_total_matches += st.match_count;
	end_sep = cfg->null_sep ? '\0' : '\n';
	if (cfg->count_only)
		printf("%s%s%ld%c", show_filename ? filename : "",
			show_filename ? ":" : "", st.match_count, end_sep);
	else if (cfg->files_with && st.match_count > 0)
		printf("%s%c", filename ? filename : "(stdin)", end_sep);
	else if (cfg->files_without && st.match_count == 0)
		printf("%s%c", filename ? filename : "(stdin)", end_sep);
	return (st.match_count);
}
