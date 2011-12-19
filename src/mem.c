#include <chpx.h>
#include <stdlib.h>
#include <assert.h>

void *
chpx_malloc(size_t size, size_t num)
{
    if(size == 0)
	return NULL;

    return malloc(size);
}

void *
chpx_calloc(size_t size, size_t num)
{
    if(size == 0)
	return NULL;

    return calloc(size, num);
}

void *
chpx_realloc(void * ptr, size_t size)
{
    assert(ptr);
    return realloc(ptr, size);
}

void
chpx_free(void * ptr)
{
    if(ptr != NULL)
	free(ptr);
}
