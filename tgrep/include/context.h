#ifndef TGREP_CONTEXT_H
# define TGREP_CONTEXT_H

# include <stddef.h>
# include "tgrep.h"

/* One buffered line in the before-context ring. */
typedef struct s_ctx_entry
{
	char	*line;		/* strdup'd, no trailing newline */
	size_t	len;		/* length of line (excluding newline) */
	long	line_num;
}	t_ctx_entry;

/* Circular buffer for before-context lines. */
typedef struct s_context
{
	t_ctx_entry	*buf;
	int			capacity;
	int			head;	/* index of the oldest entry */
	int			size;	/* current fill (0 .. capacity) */
}	t_context;

/* Allocate a context ring of 'capacity' entries. Returns NULL on failure. */
t_context	*context_init(int capacity);

/* Push a line (strdup'd) into the before-context ring. */
void		context_push(t_context *ctx, const char *line, size_t len,
				long line_num);

/*
** Print all buffered lines as context lines then clear the ring.
** Updates *last_printed to the last line number printed.
*/
void		context_drain(t_context *ctx, const char *filename,
				int show_filename, t_config *cfg, long *last_printed);

/* Free the ring and all strdup'd strings inside it. */
void		context_free(t_context *ctx);

#endif
