/* query_disc.c -- functions to find the indexes of pixels within some
 * distance from a given direction
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
#include <stdlib.h>
#include <assert.h>

void
hpix_query_disc(double theta, double phi, double radius,
		hpix_pixel_num_t ** pixels,
		size_t * num_of_matches)
{
    assert(pixels != NULL);
    assert(num_of_matches != NULL);

    *pixels = NULL;
    *num_of_matches = 0;
}

void
hpix_query_disc_inclusive(double theta, double phi, double radius,
			  hpix_pixel_num_t ** pixels,
			  size_t * num_of_matches)
{
    assert(pixels != NULL);
    assert(num_of_matches != NULL);

    *pixels = NULL;
    *num_of_matches = 0;
}
