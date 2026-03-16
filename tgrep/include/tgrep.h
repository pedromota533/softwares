#ifndef TGREP_H
# define TGREP_H

# define COLOR_NEVER   0
# define COLOR_AUTO    1
# define COLOR_ALWAYS  2

# define MAX_PATTERNS  64
# define MAX_INCLUDES  32
# define MAX_EXCLUDES  32

typedef struct s_config
{
	/* patterns */
	char	*patterns[MAX_PATTERNS];
	int		pattern_count;

	/* match options */
	int		ignore_case;	/* -i */
	int		smart_case;		/* -S: case-insensitive when pattern is all lowercase */
	int		whole_word;		/* -w */
	int		whole_line;		/* -x */
	int		invert;			/* -v */

	/* output options */
	int		line_numbers;	/* -n */
	int		only_matching;	/* -o */
	int		count_only;		/* -c */
	int		files_with;		/* -l */
	int		files_without;	/* -L */
	int		with_filename;	/* 1=force on, -1=force off, 0=auto */
	int		null_sep;		/* -Z: use \0 instead of \n after filenames */
	int		color_mode;		/* COLOR_NEVER / COLOR_AUTO / COLOR_ALWAYS */
	int		use_color;		/* resolved at runtime */
	int		json;			/* --json */
	int		stats;			/* --stats */
	int		silent;			/* -s */

	/* context */
	int		before;			/* -B */
	int		after;			/* -A */

	/* file traversal */
	int		recursive;		/* -r */
	int		max_count;		/* -m: 0 = unlimited */
	char	*include_globs[MAX_INCLUDES];
	int		include_count;
	char	*exclude_dirs[MAX_EXCLUDES];
	int		exclude_dir_count;

	/* input files (heap-allocated array of pointers into argv) */
	char	**files;
	int		file_count;

	/* accumulated stats */
	long	stat_files_searched;
	long	stat_files_matched;
	long	stat_total_matches;
}	t_config;

#endif
