#ifndef TGREP_OUTPUT_H
# define TGREP_OUTPUT_H

# include <stddef.h>
# include "tgrep.h"
# include "matcher.h"

/*
** Print the filename/line-number prefix.
** sep is ':' for match lines, '-' for context lines.
*/
void	print_prefix(const char *filename, int show_filename, long line_num,
			t_config *cfg, char sep);

/*
** Print a matched line. Handles color highlighting, -o, --json, -n, filename.
** line_len must exclude the trailing newline.
*/
void	print_match(const char *line, size_t line_len, long line_num,
			const t_match *matches, int match_count,
			const char *filename, int show_filename, t_config *cfg);

/* Print a context line (no highlight, uses '-' prefix separator). */
void	print_context_line(const char *line, size_t line_len, long line_num,
			const char *filename, int show_filename, t_config *cfg);

/* Print the context group separator '--'. */
void	print_separator(t_config *cfg);

/* Print accumulated stats to stderr. */
void	print_stats(t_config *cfg);

#endif
