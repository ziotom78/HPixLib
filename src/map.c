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

#include <hpixlib/hpix.h>
#include <assert.h>
#include <memory.h>

/**********************************************************************/


void
hpix_init_resolution_from_nside(hpix_nside_t nside,
				hpix_resolution_t * resolution)
{
    assert(resolution != NULL);

    resolution->nside            = nside;
    resolution->nside_times_two  = nside * 2;
    resolution->nside_times_four = nside * 4;

    resolution->order            = (nside & (nside-1)) ? -1 : hpix_ilog2(nside);
    resolution->pixels_per_face  = nside * nside;
    resolution->num_of_pixels    = 12 * resolution->pixels_per_face;
    resolution->ncap             = 2 * (resolution->pixels_per_face - nside);
    resolution->fact2            = 4.0 / resolution->num_of_pixels;
    resolution->fact1            = (2 * nside) * resolution->fact2;
}

/**********************************************************************/


hpix_map_t *
hpix_create_map(hpix_nside_t nside, hpix_ordering_scheme_t scheme)
{
    hpix_map_t * map = (hpix_map_t *) hpix_malloc(sizeof(hpix_map_t), 1);

    map->scheme	= scheme;
    map->coord	= HPIX_COORD_GALACTIC;

    map->pixels	= hpix_calloc(sizeof(map->pixels[0]),
			      hpix_nside_to_npixel(nside));
    map->free_pixels_flag = TRUE;

    hpix_init_resolution_from_nside(nside, &map->resolution);
    return map;
}

/**********************************************************************/


hpix_map_t *
hpix_create_map_from_array(double * array,
			   size_t num_of_elements,
			   hpix_ordering_scheme_t scheme)
{
    hpix_map_t * map = (hpix_map_t *) hpix_malloc(sizeof(hpix_map_t), 1);

    map->scheme = scheme;
    map->coord  = HPIX_COORD_GALACTIC;

    map->pixels = array;
    map->free_pixels_flag = FALSE;

    hpix_init_resolution_from_nside(hpix_npixel_to_nside(num_of_elements),
				    &map->resolution);
    return map;
}

/**********************************************************************/


void
hpix_free_map(hpix_map_t * map)
{
    if(map == NULL)
	return;

    if(map->free_pixels_flag)
	hpix_free(map->pixels);

    hpix_free(map);
}

/**********************************************************************/


hpix_map_t *
hpix_create_copy_of_map(const hpix_map_t * map)
{
    hpix_map_t * copy = hpix_create_map(hpix_map_nside(map),
					hpix_map_ordering_scheme(map));

    memcpy(hpix_map_pixels(copy),
	   hpix_map_pixels(map),
	   hpix_map_num_of_pixels(map) * sizeof(double));

    return copy;
}

/**********************************************************************/


hpix_ordering_scheme_t
hpix_map_ordering_scheme(const hpix_map_t * map)
{
    assert(map);
    return map->scheme;
}

/**********************************************************************/


hpix_coordinates_t
hpix_map_coordinate_system(const hpix_map_t * map)
{
    assert(map);
    return map->coord;
}

/**********************************************************************/


hpix_nside_t
hpix_map_nside(const hpix_map_t * map)
{
    assert(map);
    return map->resolution.nside;
}

/**********************************************************************/


double *
hpix_map_pixels(const hpix_map_t * map)
{
    assert(map);
    return map->pixels;
}

/**********************************************************************/


size_t
hpix_map_num_of_pixels(const hpix_map_t * map)
{
    assert(map);
    return hpix_nside_to_npixel(map->resolution.nside);
}

/**********************************************************************/


const hpix_resolution_t *
hpix_map_resolution(const hpix_map_t * map)
{
    assert(map);
    return &map->resolution;
}
