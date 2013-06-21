/* misc.c -- Miscellaneous functions
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
#include <math.h>

int
hpix_valid_nside(hpix_nside_t nside)
{
    return nside > 0 && (! (nside & (nside - 1)));
}

hpix_pixel_num_t
hpix_nside_to_npixel(hpix_nside_t nside)
{
    if (nside > 0)
	return 12 * nside * nside;
    else
	return 0;
}

hpix_nside_t
hpix_npixel_to_nside(hpix_pixel_num_t npixels)
{
    unsigned short  nside_estimate = sqrt(npixels / 12.0);
    if (hpix_nside_to_npixel(nside_estimate) != npixels)
	return 0;
    else
	return nside_estimate;
}

double
hpix_max_pixel_radius(hpix_nside_t nside)
{
    return 0.0;
}

