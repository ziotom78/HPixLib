/* positions.c -- functions to map positions in the sky sphere with pixels
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

void
chpx_angles_to_3dvec(double theta, double phi,
		     double * x, double * y, double * z)
{
    assert(x && y && z);

    *x = 0.0;
    *y = 0.0;
    *z = 0.0;
}

chpx_pixel_num_t
chpx_angles_to_ring_pixel(chpx_nside_t nside,
			  double theta,
			  double phi)
{
    return 0;
}

chpx_pixel_num_t
chpx_angles_to_nest_pixel(chpx_nside_t nside,
			  double theta,
			  double phi)
{
    return 0;
}

void
chpx_3dvec_to_angle(double x, double y, double z,
		    double * theta, double * phi)
{
    assert(theta && phi);

    *theta = 0.0;
    *phi = 0.0;
}

chpx_pixel_num_t
chpx_3dvec_to_ring_pixel(chpx_nside_t nside,
			 double x, double y, double z)
{
    return 0;
}

chpx_pixel_num_t
chpx_3dvec_to_nest_pixel(chpx_nside_t nside,
			 double x, double y, double z)
{
    return 0;
}

void
chpx_pixel_to_angle(chpx_nside_t nside,
		    double * theta, double * phi)
{
    assert(theta && phi);

    *theta = 0.0;
    *phi = 0.0;
}

void
chpx_ring_pixel_to_3dvec(chpx_nside_t nside,
			 double * x, double * y, double * z)
{
    assert(x && y && z);

    *x = 0.0;
    *y = 0.0;
    *z = 0.0;
}

void
chpx_nest_pixel_to_3dvec(chpx_nside_t nside,
			 double * x, double * y, double * z)
{
    assert(x && y && z);

    *x = 0.0;
    *y = 0.0;
    *z = 0.0;
}
