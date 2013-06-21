/* mem.c -- Allocation/deallocation routines
 *
 * Copyright 2011-2012 Maurizio Tomasi.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "config.h"

#include <hpixlib/hpix.h>
#include <stdlib.h>
#include <assert.h>

void *
hpix_malloc(size_t size, size_t num)
{
    if(size == 0)
	return NULL;

    return malloc(size * num);
}

void *
hpix_calloc(size_t size, size_t num)
{
    if(size == 0)
	return NULL;

    return calloc(size, num);
}

void *
hpix_realloc(void * ptr, size_t size)
{
    assert(ptr);
    return realloc(ptr, size);
}

void
hpix_free(void * ptr)
{
    if(ptr != NULL)
	free(ptr);
}
