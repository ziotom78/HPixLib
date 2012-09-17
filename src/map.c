/* map.c -- Functions dealing with the hpxlib_map_t type
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

#include <hpxlib.h>
#include <assert.h>

hpxlib_map_t *
hpxlib_create_map(hpxlib_nside_t nside, hpxlib_ordering_t ordering)
{
    hpxlib_map_t * map = (hpxlib_map_t *) hpxlib_malloc(sizeof(hpxlib_map_t), 1);

    map->order = HPXLIB_ORDER_RING;
    map->coord = HPXLIB_COORD_GALACTIC;
    map->nside = nside;
    map->pixels = hpxlib_calloc(sizeof(map->pixels[0]),
			      hpxlib_nside_to_npixel(nside));

    return map;
}

hpxlib_map_t *
hpxlib_create_map_from_array(double * array,
			     size_t num_of_elements,
			     hpxlib_ordering_t ordering)
{
    hpxlib_map_t * map = (hpxlib_map_t *) hpxlib_malloc(sizeof(hpxlib_map_t), 1);

    map->order = ordering;
    map->coord = HPXLIB_COORD_GALACTIC;
    map->nside = hpxlib_npixel_to_nside(num_of_elements);
    map->pixels = array;

    return map;
}

void
hpxlib_free_map(hpxlib_map_t * map)
{
    if(map == NULL)
	return;

    hpxlib_free(map->pixels);
    hpxlib_free(map);
}

hpxlib_ordering_t
hpxlib_map_ordering(const hpxlib_map_t * map)
{
    assert(map);
    return map->order;
}

hpxlib_coordinates_t
hpxlib_map_coordinate_system(const hpxlib_map_t * map)
{
    assert(map);
    return map->coord;
}

hpxlib_nside_t
hpxlib_map_nside(const hpxlib_map_t * map)
{
    assert(map);
    return map->nside;
}

double *
hpxlib_map_pixels(const hpxlib_map_t * map)
{
    assert(map);
    return map->pixels;
}

size_t
hpxlib_map_num_of_pixels(const hpxlib_map_t * map)
{
    assert(map);
    return hpxlib_nside_to_npixel(map->nside);
}
