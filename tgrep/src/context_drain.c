#include "context.h"
#include "output.h"

void	context_drain(t_context *ctx, const char *filename,
			int show_filename, t_config *cfg, long *last_printed)
{
	t_ctx_entry	*e;
	int			i;
	int			idx;

	if (!ctx || ctx->size == 0)
		return ;
	i = 0;
	while (i < ctx->size)
	{
		idx = (ctx->head + i) % ctx->capacity;
		e = &ctx->buf[idx];
		print_context_line(e->line, e->len, e->line_num,
			filename, show_filename, cfg);
		if (last_printed)
			*last_printed = e->line_num;
		i++;
	}
	i = 0;
	while (i < ctx->size)
	{
		idx = (ctx->head + i) % ctx->capacity;
		free(ctx->buf[idx].line);
		ctx->buf[idx].line = NULL;
		i++;
	}
	ctx->head = 0;
	ctx->size = 0;
}
