/* order_conversion.c -- functions to change the order of pixels for a
 * map from RING to NESTED and vice versa.
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

#include "chpx.h"
#include <assert.h>

chpx_map_t *
chpx_create_map(chpx_nside_t nside,
		size_t pixel_size)
{
    chpx_map_t * map = (chpx_map_t *) chpx_malloc(sizeof(chpx_map_t), 1);

    map->order = CHPX_ORDER_RING;
    map->coord = CHPX_COORD_UNKNOWN;
    map->nside = nside;

    map->pixel_size = pixel_size;
    map->pixels     = chpx_calloc(pixel_size, chpx_nside_to_npixel(nside));

    return map;
}

chpx_map_t *
chpx_create_map_from_array(void * array,
			   size_t num_of_elements,
			   size_t pixel_size)
{
    chpx_map_t * map = (chpx_map_t *) chpx_malloc(sizeof(chpx_map_t), 1);

    map->order = CHPX_ORDER_RING;
    map->coord = CHPX_COORD_UNKNOWN;
    map->nside = chpx_npixel_to_nside(num_of_elements);

    map->pixel_size = pixel_size;
    map->pixels     = array;

    return map;
}

void
chpx_free_map(chpx_map_t * map)
{
    if(map == NULL)
	return;

    chpx_free(map->pixels);
    chpx_free(map);
}

chpx_ordering_t
chpx_map_ordering(const chpx_map_t * map)
{
    return map->order;
}

chpx_coordinates_t
chpx_map_coordinate_system(const chpx_map_t * map)
{
    return map->coord;
}

chpx_nside_t
chpx_map_nside(const chpx_map_t * map)
{
    return map->nside;
}

size_t
chpx_map_pixel_size(const chpx_map_t * map)
{
    return map->pixel_size;
}

void *
chpx_map_pixels(const chpx_map_t * map)
{
    return map->pixels;
}
