#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "matcher.h"

static int	is_all_lowercase(const char *pat)
{
	while (*pat)
	{
		if (isupper((unsigned char)*pat))
			return (0);
		pat++;
	}
	return (1);
}

static char	*build_pattern(const char *src, t_config *cfg)
{
	size_t	len;
	char	*pat;
	size_t	prefix_len;
	size_t	suffix_len;

	len = strlen(src);
	prefix_len = cfg->whole_word ? 4 : (cfg->whole_line ? 4 : 0);
	suffix_len = cfg->whole_word ? 4 : (cfg->whole_line ? 2 : 0);
	pat = malloc(len + prefix_len + suffix_len + 1);
	if (!pat)
		return (NULL);
	pat[0] = '\0';
	if (cfg->whole_word)
		strcpy(pat, "\\b(");
	else if (cfg->whole_line)
		strcpy(pat, "^(?:");
	strcat(pat, src);
	if (cfg->whole_word)
		strcat(pat, ")\\b");
	else if (cfg->whole_line)
		strcat(pat, ")$");
	return (pat);
}

t_matcher	*matcher_create(t_config *cfg)
{
	t_matcher		*m;
	uint32_t		flags;
	char			*pat;
	int				errcode;
	PCRE2_SIZE		erroffset;
	int				i;

	m = malloc(sizeof(t_matcher));
	if (!m)
		return (NULL);
	m->codes = calloc(cfg->pattern_count, sizeof(pcre2_code *));
	if (!m->codes)
	{
		free(m);
		return (NULL);
	}
	m->count = cfg->pattern_count;
	i = 0;
	while (i < cfg->pattern_count)
	{
		flags = PCRE2_MULTILINE;
		if (cfg->ignore_case
			|| (cfg->smart_case && is_all_lowercase(cfg->patterns[i])))
			flags |= PCRE2_CASELESS;
		pat = build_pattern(cfg->patterns[i], cfg);
		if (!pat)
		{
			matcher_free(m);
			return (NULL);
		}
		m->codes[i] = pcre2_compile((PCRE2_SPTR)pat, PCRE2_ZERO_TERMINATED,
				flags, &errcode, &erroffset, NULL);
		free(pat);
		if (!m->codes[i])
		{
			PCRE2_UCHAR errbuf[256];
			pcre2_get_error_message(errcode, errbuf, sizeof(errbuf));
			fprintf(stderr, "tgrep: invalid pattern '%s': %s (offset %zu)\n",
				cfg->patterns[i], (char *)errbuf, erroffset);
			matcher_free(m);
			return (NULL);
		}
		i++;
	}
	return (m);
}
