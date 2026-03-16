#include <stdlib.h>
#include "context.h"

t_context	*context_init(int capacity)
{
	t_context	*ctx;

	if (capacity <= 0)
		return (NULL);
	ctx = malloc(sizeof(t_context));
	if (!ctx)
		return (NULL);
	ctx->buf = calloc(capacity, sizeof(t_ctx_entry));
	if (!ctx->buf)
	{
		free(ctx);
		return (NULL);
	}
	ctx->capacity = capacity;
	ctx->head = 0;
	ctx->size = 0;
	return (ctx);
}
