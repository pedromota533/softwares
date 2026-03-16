#ifndef TGREP_SEARCH_H
# define TGREP_SEARCH_H

# include "tgrep.h"
# include "matcher.h"

/*
** Search a single file for matches.
** filename == NULL means read from stdin.
** show_filename: whether to print the filename prefix in output.
** Returns the number of matches found.
*/
long	search_file(const char *filename, int show_filename,
			t_matcher *matcher, t_config *cfg);

/*
** Recursively search a directory tree for matches.
** Returns the total number of matches found.
*/
long	search_dir(const char *path, t_matcher *matcher, t_config *cfg);

/* Returns 1 if filename matches at least one --include glob (or none set). */
int		glob_match(const char *filename, t_config *cfg);

/* Returns 1 if the first bytes of the file contain a null byte (binary). */
int		is_binary(const char *filename);

#endif
