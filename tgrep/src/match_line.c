#include <stdlib.h>
#include <string.h>
#include "matcher.h"

#define MAX_RAW_MATCHES 2048

static int	cmp_match(const void *a, const void *b)
{
	const t_match	*ma;
	const t_match	*mb;

	ma = (const t_match *)a;
	mb = (const t_match *)b;
	if (ma->start != mb->start)
		return ((ma->start < mb->start) ? -1 : 1);
	return ((ma->end > mb->end) ? -1 : 1);
}

static int	collect_pattern(pcre2_code *code, const char *line, size_t len,
				t_match *raw, int *raw_count)
{
	pcre2_match_data	*mdata;
	PCRE2_SIZE			*ov;
	size_t				offset;
	int					rc;

	mdata = pcre2_match_data_create_from_pattern(code, NULL);
	if (!mdata)
		return (-1);
	offset = 0;
	while (offset <= len && *raw_count < MAX_RAW_MATCHES)
	{
		rc = pcre2_match(code, (PCRE2_SPTR)line, len, offset, 0, mdata, NULL);
		if (rc < 0)
			break ;
		ov = pcre2_get_ovector_pointer(mdata);
		if (ov[0] == ov[1])
		{
			offset = ov[1] + 1;
			continue ;
		}
		raw[*raw_count].start = ov[0];
		raw[*raw_count].end = ov[1];
		(*raw_count)++;
		offset = ov[1];
	}
	pcre2_match_data_free(mdata);
	return (0);
}

t_match	*match_line(t_matcher *m, const char *line, size_t len, int *out_count)
{
	t_match	raw[MAX_RAW_MATCHES];
	t_match	*merged;
	int		raw_count;
	int		mc;
	int		i;

	*out_count = 0;
	raw_count = 0;
	i = 0;
	while (i < m->count)
	{
		if (collect_pattern(m->codes[i], line, len, raw, &raw_count) < 0)
			return (NULL);
		i++;
	}
	if (raw_count == 0)
		return (NULL);
	qsort(raw, raw_count, sizeof(t_match), cmp_match);
	merged = malloc(raw_count * sizeof(t_match));
	if (!merged)
		return (NULL);
	mc = 0;
	merged[mc++] = raw[0];
	i = 1;
	while (i < raw_count)
	{
		if (raw[i].start < merged[mc - 1].end)
		{
			if (raw[i].end > merged[mc - 1].end)
				merged[mc - 1].end = raw[i].end;
		}
		else
			merged[mc++] = raw[i];
		i++;
	}
	*out_count = mc;
	return (merged);
}
