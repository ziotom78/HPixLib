/* math.c -- Functions that do some calculation on the pixels of a map
 *
 * Copyright 2011-2013 Maurizio Tomasi.
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
#include <math.h>

double
hpix_average_pixel_value(const hpix_map_t * map)
{
    size_t good_pixels = 0;
    double sum_of_pixels = 0.0;
    double * pixels = hpix_map_pixels(map);
    size_t num_of_pixels = hpix_map_num_of_pixels(map);
    for(size_t idx = 0; idx < num_of_pixels; ++idx)
    {
	if(! HPIX_IS_MASKED(pixels[idx]))
	{
	    ++good_pixels;
	    sum_of_pixels += pixels[idx];
	} else {
	    pixels[idx] = NAN;
	}
    }
    
    return sum_of_pixels / good_pixels;
}

/******************************************************************************/

void
hpix_scale_pixels_by_constant_inplace(hpix_map_t * map, double constant)
{
    /* Multiply the pixels in the map by `scale_factor` */
    double * pixels = hpix_map_pixels(map);
    size_t num_of_pixels = hpix_map_num_of_pixels(map);
    for(size_t idx = 0; idx < num_of_pixels; ++idx)
    {
	if(! HPIX_IS_MASKED(pixels[idx]))
	    pixels[idx] *= constant;
    }
}

/******************************************************************************/

void
hpix_add_constant_to_pixels_inplace(hpix_map_t * map, double constant)
{
    /* Multiply the pixels in the map by `scale_factor` */
    double * pixels = hpix_map_pixels(map);
    size_t num_of_pixels = hpix_map_num_of_pixels(map);
    for(size_t idx = 0; idx < num_of_pixels; ++idx)
    {
	if(! HPIX_IS_MASKED(pixels[idx]))
	    pixels[idx] += constant;
    }
}

/******************************************************************************/

void
hpix_remove_monopole_from_map_inplace(hpix_map_t * map)
{
    double average = hpix_average_pixel_value(map);
    hpix_add_constant_to_pixels_inplace(map, -average);
}
