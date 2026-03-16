#include <stdio.h>
#include "output.h"
#include "color.h"

static void	json_escape(const char *s, size_t len)
{
	size_t			i;
	unsigned char	c;

	i = 0;
	while (i < len)
	{
		c = (unsigned char)s[i];
		if (c == '"')
			fputs("\\\"", stdout);
		else if (c == '\\')
			fputs("\\\\", stdout);
		else if (c == '\n')
			fputs("\\n", stdout);
		else if (c == '\r')
			fputs("\\r", stdout);
		else if (c == '\t')
			fputs("\\t", stdout);
		else if (c < 0x20)
			printf("\\u%04x", c);
		else
			fputc(c, stdout);
		i++;
	}
}

static void	print_highlighted(const char *line, size_t len,
				const t_match *matches, int mc, int use_color)
{
	size_t	pos;
	int		i;

	pos = 0;
	i = 0;
	while (i < mc)
	{
		if (matches[i].start > pos)
			fwrite(line + pos, 1, matches[i].start - pos, stdout);
		if (use_color)
			fputs(COL_MATCH, stdout);
		fwrite(line + matches[i].start, 1,
			matches[i].end - matches[i].start, stdout);
		if (use_color)
			fputs(COL_RESET, stdout);
		pos = matches[i].end;
		i++;
	}
	if (pos < len)
		fwrite(line + pos, 1, len - pos, stdout);
}

static void	print_json(const char *line, size_t len, long line_num,
				const t_match *matches, int mc, const char *filename,
				int only_matching)
{
	int	i;

	if (only_matching)
	{
		i = 0;
		while (i < mc)
		{
			printf("{\"file\":\"%s\",\"line\":%ld,\"col\":%zu,\"match\":\"",
				filename ? filename : "", line_num, matches[i].start + 1);
			json_escape(line + matches[i].start,
				matches[i].end - matches[i].start);
			fputs("\"}\n", stdout);
			i++;
		}
		return ;
	}
	printf("{\"file\":\"%s\",\"line\":%ld", filename ? filename : "", line_num);
	if (mc > 0)
		printf(",\"col\":%zu", matches[0].start + 1);
	fputs(",\"text\":\"", stdout);
	json_escape(line, len);
	fputs("\"}\n", stdout);
}

void	print_match(const char *line, size_t line_len, long line_num,
			const t_match *matches, int match_count,
			const char *filename, int show_filename, t_config *cfg)
{
	int	i;

	if (cfg->json)
	{
		print_json(line, line_len, line_num, matches, match_count,
			filename, cfg->only_matching);
		return ;
	}
	if (cfg->only_matching)
	{
		i = 0;
		while (i < match_count)
		{
			print_prefix(filename, show_filename, line_num, cfg, ':');
			fwrite(line + matches[i].start, 1,
				matches[i].end - matches[i].start, stdout);
			fputc(cfg->null_sep ? '\0' : '\n', stdout);
			i++;
		}
		return ;
	}
	print_prefix(filename, show_filename, line_num, cfg, ':');
	print_highlighted(line, line_len, matches, match_count, cfg->use_color);
	fputc('\n', stdout);
}
