#include <stdlib.h>
#include "context.h"

void	context_free(t_context *ctx)
{
	int	i;
	int	idx;

	if (!ctx)
		return ;
	i = 0;
	while (i < ctx->size)
	{
		idx = (ctx->head + i) % ctx->capacity;
		free(ctx->buf[idx].line);
		i++;
	}
	free(ctx->buf);
	free(ctx);
}
