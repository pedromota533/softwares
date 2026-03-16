#ifndef TGREP_COLOR_H
# define TGREP_COLOR_H

# define COL_RESET    "\033[0m"
# define COL_MATCH    "\033[1;31m"	/* bold red   — matched text      */
# define COL_FILENAME "\033[1;35m"	/* bold magenta — filename prefix  */
# define COL_LINENO   "\033[1;36m"	/* bold cyan  — line number prefix */
# define COL_SEP      "\033[36m"	/* cyan       — context separator  */

# include "tgrep.h"

/* Resolve cfg->use_color from cfg->color_mode and whether stdout is a TTY. */
void	color_resolve(t_config *cfg);

#endif
