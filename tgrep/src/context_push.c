#include <stdlib.h>
#include <string.h>
#include "context.h"

void	context_push(t_context *ctx, const char *line, size_t len,
			long line_num)
{
	int		idx;
	char	*copy;

	copy = malloc(len + 1);
	if (!copy)
		return ;
	memcpy(copy, line, len);
	copy[len] = '\0';
	if (ctx->size < ctx->capacity)
	{
		idx = (ctx->head + ctx->size) % ctx->capacity;
		ctx->size++;
	}
	else
	{
		idx = ctx->head;
		free(ctx->buf[ctx->head].line);
		ctx->head = (ctx->head + 1) % ctx->capacity;
	}
	ctx->buf[idx].line = copy;
	ctx->buf[idx].len = len;
	ctx->buf[idx].line_num = line_num;
}
