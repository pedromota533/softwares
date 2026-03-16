#ifndef TGREP_MATCHER_H
# define TGREP_MATCHER_H

# include <stddef.h>
# include <pcre2.h>
# include "tgrep.h"

/* A single match interval [start, end) within a line. */
typedef struct s_match
{
	size_t	start;
	size_t	end;
}	t_match;

/* Set of compiled PCRE2 patterns. */
typedef struct s_matcher
{
	pcre2_code	**codes;
	int			count;
}	t_matcher;

/* Compile all patterns in cfg. Returns NULL on error. */
t_matcher	*matcher_create(t_config *cfg);

/* Free all compiled patterns and the matcher itself. */
void		matcher_free(t_matcher *m);

/*
** Find all non-overlapping matches of any pattern in [line, line+len).
** Returns a malloc'd sorted array of t_match, sets *out_count.
** Returns NULL with *out_count == 0 when there are no matches.
*/
t_match		*match_line(t_matcher *m, const char *line, size_t len,
				int *out_count);

#endif
