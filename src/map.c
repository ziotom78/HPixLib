/* map.c -- Functions dealing with the hpix_map_t type
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

#include <hpix.h>
#include <assert.h>

hpix_map_t *
hpix_create_map(hpix_nside_t nside, hpix_ordering_t ordering)
{
    hpix_map_t * map = (hpix_map_t *) hpix_malloc(sizeof(hpix_map_t), 1);

    map->order = HPXLIB_ORDER_RING;
    map->coord = HPXLIB_COORD_GALACTIC;
    map->nside = nside;
    map->pixels = hpix_calloc(sizeof(map->pixels[0]),
			      hpix_nside_to_npixel(nside));

    return map;
}

hpix_map_t *
hpix_create_map_from_array(double * array,
			   size_t num_of_elements,
			   hpix_ordering_t ordering)
{
    hpix_map_t * map = (hpix_map_t *) hpix_malloc(sizeof(hpix_map_t), 1);

    map->order = ordering;
    map->coord = HPXLIB_COORD_GALACTIC;
    map->nside = hpix_npixel_to_nside(num_of_elements);
    map->pixels = array;

    return map;
}

void
hpix_free_map(hpix_map_t * map)
{
    if(map == NULL)
	return;

    hpix_free(map->pixels);
    hpix_free(map);
}

hpix_ordering_t
hpix_map_ordering(const hpix_map_t * map)
{
    assert(map);
    return map->order;
}

hpix_coordinates_t
hpix_map_coordinate_system(const hpix_map_t * map)
{
    assert(map);
    return map->coord;
}

hpix_nside_t
hpix_map_nside(const hpix_map_t * map)
{
    assert(map);
    return map->nside;
}

double *
hpix_map_pixels(const hpix_map_t * map)
{
    assert(map);
    return map->pixels;
}

size_t
hpix_map_num_of_pixels(const hpix_map_t * map)
{
    assert(map);
    return hpix_nside_to_npixel(map->nside);
}
